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

#include "players_repository.h"

#include <easylogging++.h>

#include "repository_exceptions.h"
#include <macros.h>

using namespace std;
using namespace experimental;
using namespace roa;
using namespace pqxx;

players_repository::players_repository(idatabase_pool& database_pool) : repository(database_pool) {

}

players_repository::players_repository(players_repository &repo) : repository(repo._database_pool, unique_ptr<idatabase_connection>(repo._connection.release())) {

}

players_repository::players_repository(players_repository &&repo) : repository(repo._database_pool, move(repo._connection)) {

}

players_repository::~players_repository() {

}

unique_ptr<idatabase_transaction> players_repository::create_transaction() {
    return repository::create_transaction();
}

bool players_repository::insert_or_update_player(player &plyr, unique_ptr<idatabase_transaction> const &transaction) {

    auto result = transaction->execute(
            "INSERT INTO players (user_id, location_id, player_name) VALUES (" + to_string(plyr.user_id) +
            ", " + to_string(plyr.location_id) + ", '" + transaction->escape(plyr.name) + "') ON CONFLICT (player_name) DO UPDATE SET"
            " user_id = " + to_string(plyr.user_id) + ", location_id = " + to_string(plyr.location_id) +
            " RETURNING xmax, id");

    if(unlikely(result.size() == 0)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    plyr.id = result[0][1].as<uint64_t>();

    if(result[0]["xmax"].as<string>() == "0") {
        LOG(DEBUG) << NAMEOF(players_repository::insert_or_update_player) << " inserted player with id " << plyr.id;
        return true;
    }
    LOG(DEBUG) << NAMEOF(players_repository::insert_or_update_player) << " updated player with id " << plyr.id;
    return false;
}

void players_repository::insert_player_at_start_location(player &plyr,
                                                         std::unique_ptr<idatabase_transaction> const &transaction) {
    // Currently this requires three queries, but it is technically possible to have postgresql cast
    // the setting value to an int.
    auto setting_result = transaction->execute("SELECT value FROM settings WHERE setting_name='player_start_script_zone'");

    if(unlikely(setting_result.size() != 1)) {
        throw unexpected_result_error("expected exactly one setting_result"s);
    }

    auto script_zone_id = setting_result[0][0].as<string>();

    if(unlikely(script_zone_id.length() == 0)) {
        throw unexpected_result_error("expected script_zone_id to contain something"s);
    }

    auto loc_result = transaction->execute("INSERT INTO locations AS l (map_id, x, y) SELECT z.map_id, z.x, z.y "
                                           "FROM script_zones z WHERE z.id = " + transaction->escape(script_zone_id) + " RETURNING l.id");

    if(unlikely(loc_result.size() != 1)) {
        throw unexpected_result_error("expected exactly one loc_result"s);
    }

    uint64_t loc_id = loc_result[0][0].as<uint32_t>();

    auto result = transaction->execute(
            "INSERT INTO players (user_id, location_id, player_name) VALUES (" + to_string(plyr.user_id) +
            ", " + to_string(loc_id) +  ", '" + transaction->escape(plyr.name) + "') RETURNING id");

    if(unlikely(result.size() != 1)) {
        throw unexpected_result_error("expected exactly one result"s);
    }

    plyr.id = result[0][0].as<uint64_t>();

    LOG(DEBUG) << NAMEOF(players_repository::insert_player_at_start_location) << " inserted player with id " << plyr.id << " in location " << loc_id;
}

void players_repository::update_player(player &plyr, unique_ptr<idatabase_transaction> const &transaction) {

    transaction->execute(
        "UPDATE players SET user_id = " + to_string(plyr.user_id) + ", location_id = " + to_string(plyr.location_id) +
        " WHERE id = " + to_string(plyr.id));

    LOG(DEBUG) << NAMEOF(players_repository::update_player) << " updated player with id " << plyr.id;
}

STD_OPTIONAL<player> players_repository::get_player(string const &name, included_tables includes,
                                                    unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT * FROM players WHERE player_name = '" + transaction->escape(name) + "'");

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found no player by name " << name;
        return {};
    }

    auto ret = make_optional<player>({result[0]["id"].as<uint64_t>(), result[0]["user_id"].as<uint64_t>(),
                                  result[0]["location_id"].as<uint64_t>(), result[0]["player_name"].as<string>()});

    LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found player by name with id " << ret->id;

    return ret;
}

STD_OPTIONAL<player> players_repository::get_player(uint64_t id, included_tables includes,
                                                    unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT * FROM players WHERE id = " + to_string(id));

    if(result.size() == 0) {
        LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found no player by id " << id;
        return {};
    }

    auto ret = make_optional<player>({result[0]["id"].as<uint64_t>(), result[0]["user_id"].as<uint64_t>(),
                                      result[0]["location_id"].as<uint64_t>(), result[0]["player_name"].as<string>()});

    LOG(DEBUG) << NAMEOF(players_repository::get_player) << " found player by id with id " << ret->id;

    return ret;
}

vector<player> players_repository::get_player_by_user_id(uint64_t user_id, included_tables includes,
                                                         unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT * FROM players WHERE user_id = " + to_string(user_id));

    LOG(DEBUG) << NAMEOF(players_repository::get_player_by_user_id) << " contains " << result.size() << " entries";

    vector<player> players;
    players.resize(result.size());

    for(auto const & res : result) {
        players.emplace_back<player>({res["id"].as<uint64_t>(), res["user_id"].as<uint64_t>(),
                              res["location_id"].as<uint64_t>(), res["player_name"].as<string>()});
    }

    return players;
}
