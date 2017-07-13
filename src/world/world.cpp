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

#include <repositories/scripts_repository.h>
#include <boost/di.hpp>
#include <ecs/systems/script_system.h>
#include "world.h"
#include "../ecs/components/position_component.h"
#include "../ecs/components/map_component.h"
#include "../ecs/components/tile_component.h"
#include "../ecs/components/script_component.h"
#include <easylogging++.h>
#include <macros.h>

using namespace std;
using namespace roa;
using namespace entityx;

void world::do_tick(uint32_t tick_length) {
    LOG(DEBUG) << NAMEOF(world::do_tick) << " starting tick";
    _ex.systems.update_all(tick_length);
    LOG(DEBUG) << NAMEOF(world::do_tick) << " finished tick";
}

void world::load_from_database(shared_ptr<idatabase_pool> db_pool, Config& config) {

    LOG(INFO) << NAMEOF(world::load_from_database) << " loading world from db";

    auto repo_injector = boost::di::make_injector(
            boost::di::bind<idatabase_transaction>.to<database_transaction>(),
            boost::di::bind<idatabase_connection>.to<database_connection>(),
            boost::di::bind<idatabase_pool>.to(db_pool),
            boost::di::bind<iscripts_repository>.to<scripts_repository>()
    );

    _ex.systems.add<script_system>(config);
    _ex.systems.configure();

    Entity map_entity = _ex.entities.create();
    map_entity.assign<map_component>(0, 64, 64, 640, 640, 1, 1, 10);
    ComponentHandle<map_component> mc = map_entity.component<map_component>();

    mc->tilesets.emplace_back(1, "terrain.png"s, 64, 64, 1536, 2560);

    mc->tiles.resize(1);
    mc->tiles[0].resize(100);

    for(uint32_t x = 0; x < 25; x++) {
        for(uint32_t y = 0; y < 25; y++) {
            Entity tile_entity = _ex.entities.create();
            tile_entity.assign<tile_component>(map_entity.id().id(), y+1);
            mc->tiles[0][x].push_back(tile_entity);
        }
    }

    for(uint32_t x = 0; x < 10; x++) {
        Entity npc_entity = _ex.entities.create();
        npc_entity.assign<position_component>(x, 0, mc->id);
        mc->npcs.push_back(npc_entity);
    }

    {
        scripts_repository scripts_repo = repo_injector.create<scripts_repository>();
        auto transaction = scripts_repo.create_transaction();
        auto script = scripts_repo.get_script("test_tile_script", get<1>(transaction));

        for(uint32_t x = 0; x < 25; x++) {
            for(uint32_t y = 0; y < 25; y++) {
                mc->tiles[0][x][y].assign<script_component>(script->name, script->text, 500, 500, trigger_type_enum::looped, false);
            }
        }
    }

    LOG(INFO) << NAMEOF(world::load_from_database) << " loaded world from db with " << _ex.entities.size() << " entities";
}
