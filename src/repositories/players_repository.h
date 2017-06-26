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

#pragma once

#include <string>
#include <custom_optional.h>
#include "repository.h"

namespace roa {
    struct player_location {
        uint32_t x;
        uint32_t y;
        std::string map_name;

        player_location() = default;
        player_location(uint32_t x, uint32_t y, std::string map_name) : x(x), y(y), map_name(map_name) {}
    };

    struct player_stat {
        uint64_t id;
        std::string name;
        uint64_t value;

        player_stat() = default;
        player_stat(uint64_t id, std::string name, uint64_t value) : id(id), name(name), value(value) {}
    };

    struct player_item {
        uint64_t id;
        std::string name;

        player_item() = default;
        player_item(uint64_t id, std::string name) : id(id), name(name) {}
    };

    struct player {
        uint64_t id;
        uint64_t user_id;
        uint64_t location_id;
        std::string name;
        STD_OPTIONAL<player_location> location;
        std::vector<player_stat> stats;
        std::vector<player_item> items;
    };

    enum class included_tables : int
    {
        none,
        stats,
        location,
        items,
        all
    };

    class iplayers_repository : public irepository {
    public:
        virtual ~iplayers_repository() = default;

        /**
         * Insert or update player
         * @param plyr
         * @param transaction
         * @return true if inserted, false if updated
         */
        virtual bool insert_or_update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Insert or update player at the script_zone available from the settings table
         * @param plyr
         * @param transaction
         * @return true if inserted, false if updated
         */
        virtual void insert_player_at_start_location(player &plyr,
                                                     std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update a player
         * @param plyr
         * @param transaction
         */
        virtual void update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get player by name
         * @param name name of player
         * @param transaction
         * @return player if found, {} if not
         */
        virtual STD_OPTIONAL<player> get_player(std::string const & name, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get player by player id
         * @param id id of player
         * @param transaction
         * @return player if found, {} if not
         */
        virtual STD_OPTIONAL<player> get_player(uint64_t id, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get players by user id
         * @param user_id id of player
         * @param transaction
         * @return players if found, {} if not
         */
        virtual std::vector<player> get_players_by_user_id(uint64_t user_id, included_tables includes,
                                                           std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class players_repository : public repository, public iplayers_repository {
    public:
        explicit players_repository(std::shared_ptr<idatabase_pool> database_pool);
        players_repository(players_repository &repo);
        players_repository(players_repository &&repo);
        ~players_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        bool insert_or_update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void insert_player_at_start_location(player &plyr,
                                             std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<player> get_player(std::string const & name, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<player> get_player(uint64_t id, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) override;
        std::vector<player> get_players_by_user_id(uint64_t user_id, included_tables includes,
                                                   std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}