/*
    Realm of Aesir backend
    Copyright (C) 2016  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <easylogging++.h>
#include <json.hpp>
#include <kafka_consumer.h>
#include <kafka_producer.h>
#include <admin_messages/admin_quit_message.h>

#include <signal.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <exceptions.h>
#include <roa_di.h>
#include <macros.h>
#include <database_pool.h>
#include <repositories/locations_repository.h>
#include <repositories/maps_repository.h>
#include <repositories/script_zones_repository.h>
#include <repositories/settings_repository.h>
#include "message_handlers/message_dispatcher.h"
#include "message_handlers/world/world_create_character_handler.h"
#include "database_transaction.h"
#include "config.h"

using namespace std;
using namespace roa;
using namespace pqxx;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

using json = nlohmann::json;


INITIALIZE_EASYLOGGINGPP

atomic<bool> quit{false};

void on_sigint(int sig) {
    quit = true;
}

void init_extras() noexcept {
    ios::sync_with_stdio(false);
    signal(SIGINT, on_sigint);
}

void init_logger(Config const config) noexcept {
    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    if(!config.debug_level.empty()) {
        if(config.debug_level == "error") {
            defaultConf.set(el::Level::Warning, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        } else if(config.debug_level == "warning") {
            defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        } else if(config.debug_level == "info") {
            defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        } else if(config.debug_level == "debug") {
            defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "false");
        }
    }
    el::Loggers::reconfigureAllLoggers(defaultConf);
    LOG(INFO) << "debug level: " << config.debug_level;
}



STD_OPTIONAL<Config> parse_env_file() {
    string env_contents;
    ifstream env(".env");

    if(!env) {
        LOG(ERROR) << "[main] no .env file found. Please make one.";
        return {};
    }

    env.seekg(0, ios::end);
    env_contents.resize(env.tellg());
    env.seekg(0, ios::beg);
    env.read(&env_contents[0], env_contents.size());
    env.close();

    auto env_json = json::parse(env_contents);
    Config config;

    try {
        config.broker_list = env_json["BROKER_LIST"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] BROKER_LIST missing in .env file.";
        return {};
    }

    try {
        config.group_id = env_json["GROUP_ID"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] GROUP_ID missing in .env file.";
        return {};
    }

    try {
        config.server_id = env_json["SERVER_ID"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] SERVER_ID missing in .env file.";
        return {};
    }

    if(config.server_id == 0) {
        LOG(ERROR) << "[main] SERVER_ID has to be greater than 0";
        return {};
    }

    try {
        config.connection_string = env_json["CONNECTION_STRING"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] CONNECTION_STRING missing in .env file.";
        return {};
    }

    try {
        config.debug_level = env_json["DEBUG_LEVEL"];
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] DEBUG_LEVEL missing in .env file.";
        return {};
    }

    return config;
}

int main() {
    Config config;
    try {
        auto config_opt = parse_env_file();
        if(!config_opt) {
            return 1;
        }
        config = config_opt.value();
    } catch (const std::exception& e) {
        LOG(ERROR) << "[main] .env file is malformed json.";
        return 1;
    }

    init_logger(config);
    init_extras();

    database_pool db_pool;
    db_pool.create_connections(config.connection_string, 2);
    auto common_injector = create_common_di_injector();
    auto backend_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<ilocations_repository>.to<locations_repository>(),
            boost::di::bind<imaps_repository>.to<maps_repository>(),
            boost::di::bind<iplayers_repository>.to<players_repository>(),
            boost::di::bind<iscript_zones_repository>.to<script_zones_repository>(),
            boost::di::bind<isettings_repository>.to<settings_repository>()
    );

    auto producer = common_injector.create<shared_ptr<ikafka_producer<false>>>();
    auto backend_consumer = common_injector.create<shared_ptr<ikafka_consumer<false>>>();
    auto players_repo = backend_injector.create<players_repository>();
    backend_consumer->start(config.broker_list, config.group_id, std::vector<std::string>{"server-" + to_string(config.server_id), "world_messages", "broadcast"});
    producer->start(config.broker_list);


    try {
        message_dispatcher<false> world_server_msg_dispatcher;

        world_server_msg_dispatcher.register_handler<world_create_character_handler>(config, players_repo, producer);

        LOG(INFO) << "[main] starting main thread";

        while (!quit) {
            try {
                producer->poll(10);
                auto msg = backend_consumer->try_get_message(10);
                if (get<1>(msg)) {

                    world_server_msg_dispatcher.trigger_handler(msg);
                }
            } catch (serialization_exception &e) {
                cout << "[main] received exception " << e.what() << endl;
            }
        }

        LOG(INFO) << "[main] closing";

        producer->close();
        backend_consumer->close();
    } catch (const runtime_error& e) {
        LOG(ERROR) << "[main] error: " << typeid(e).name() << "-" << e.what();
    }

    return 0;
}
