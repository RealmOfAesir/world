/*
    Realm of Aesir
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

#include <catch.hpp>
#include <roa_di.h>
#include <easylogging++.h>
#include <database_transaction.h>
#include "test_helpers/startup_helper.h"
#include "../src/repositories/players_repository.h"
#include "../src/repositories/maps_repository.h"
#include "../src/repositories/locations_repository.h"
#include "../src/repositories/settings_repository.h"
#include "../src/repositories/script_zones_repository.h"

using namespace std;
using namespace roa;

auto backend_injector = boost::di::make_injector(
        boost::di::bind<idatabase_transaction>.to<database_transaction>(),
        boost::di::bind<idatabase_connection>.to<database_connection>(),
        boost::di::bind<idatabase_pool>.to(db_pool),
        boost::di::bind<irepository>.to<repository>(),
        boost::di::bind<iplayers_repository>.to<players_repository>(),
        boost::di::bind<imaps_repository>.to<maps_repository>(),
        boost::di::bind<ilocations_repository>.to<locations_repository>(),
        boost::di::bind<isettings_repository>.to<settings_repository>(),
        boost::di::bind<iscript_zones_repository>.to<script_zones_repository>());

TEST_CASE("players repository tests") {
    players_repository players_repo = backend_injector.create<players_repository>();
    maps_repository maps_repo = backend_injector.create<maps_repository>();
    locations_repository locations_repo = backend_injector.create<locations_repository>();
    auto transaction = players_repo.create_transaction();

    SECTION( "player inserted correctly" ) {
        roa::map _map{0, "map_name"s};

        maps_repo.insert_map(_map, get<1>(transaction));

        location loc{0, _map.id, 0, 0};
        locations_repo.insert_location(loc, get<1>(transaction));

        player plyr{0, 1, loc.id, "john doe"s};
        players_repo.insert_or_update_player(plyr, get<1>(transaction));

        auto plyr2 = players_repo.get_player(plyr.id, included_tables::none, get<1>(transaction));
        REQUIRE(plyr2);
        REQUIRE(plyr2->location_id == plyr.location_id);
        REQUIRE(plyr2->user_id == plyr.user_id);
        REQUIRE(plyr2->name == plyr.name);
    }

    SECTION( "player inserted correctly at script zone" ) {
        settings_repository settings_repo = backend_injector.create<settings_repository>();
        script_zones_repository script_zones_repo = backend_injector.create<script_zones_repository>();

        roa::map _map{0, "map_name"s};
        maps_repo.insert_map(_map, get<1>(transaction));

        script_zone zone{0, "zone_name", _map.id, 0, 0, 1, 1};
        script_zones_repo.insert_script_zone(zone, get<1>(transaction));

        auto sett = settings_repo.get_setting("player_start_script_zone", get<1>(transaction));
        sett->value = to_string(zone.id);
        settings_repo.insert_or_update_setting(sett.value(), get<1>(transaction));

        player plyr{0, 1, 0, "john doe"s};
        players_repo.insert_player_at_start_location(plyr, get<1>(transaction));

        auto plyr2 = players_repo.get_player(plyr.id, included_tables::none, get<1>(transaction));
        REQUIRE(plyr2);
        REQUIRE(plyr2->location_id > 0);

        auto loc = locations_repo.get_location(plyr2->location_id, get<1>(transaction));
        REQUIRE(loc);
        REQUIRE(plyr2->location_id == loc->id);
        REQUIRE(plyr2->user_id == plyr.user_id);
        REQUIRE(plyr2->name == plyr.name);
    }
}