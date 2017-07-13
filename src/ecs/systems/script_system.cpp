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

#include "script_system.h"
#include "../components/script_component.h"
#include "../components/tile_component.h"
#include "../components/character_component.h"
#include <easylogging++.h>
#include <macros.h>
#include <lua/lua_interop.h>
#include <ecs/components/position_component.h>
#include <ecs/components/map_component.h>
#include <custom_optional.h>

using namespace std;
using namespace experimental;
using namespace roa;

queue<shared_ptr<event_type>> _script_event_queue;

enum entity_types {
    tile = 1,
    character = 2
};

STD_OPTIONAL<entityx::Entity> get_map_entity_from_id(entityx::EntityManager &es, uint32_t map_id) {
    STD_OPTIONAL<entityx::Entity> ret;
    es.each<map_component>([&](entityx::Entity entity, map_component& map) {
        if(map.id == map_id) {
            ret = make_optional(entity);
        }
    });

    return ret;
}

void script_system::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {
    int updated_scripts = 0;
    auto start = chrono::high_resolution_clock::now();
    es.each<script_component>([&](entityx::Entity entity, script_component& script) {
        if(script.execute_in_ms <= dt) {

            auto tile_handle = entity.component<tile_component>();
            auto character_handle = entity.component<character_component>();

            LOG(DEBUG) << NAMEOF(script_system::update) << " setting up script";

            auto L = load_script_with_libraries(script.script_text);

            lua_newtable(L);

            lua_pushstring(L, "debug");
            lua_pushboolean(L, script.debug);
            lua_rawset(L, -3);

            lua_pushstring(L, "library_debug");
            lua_pushboolean(L, _config.debug_roa_library);
            lua_rawset(L, -3);

            lua_setglobal(L, "roa_settings");

            lua_newtable(L);

            if(tile_handle) {
                lua_pushstring(L, "tile_id");
                lua_pushinteger(L, tile_handle->tile_id);
                lua_rawset(L, -3);

                lua_pushstring(L, "id");
                lua_pushinteger(L, entity.id().id());
                lua_rawset(L, -3);

                lua_pushstring(L, "type");
                lua_pushinteger(L, entity_types::tile);
                lua_rawset(L, -3);
            } else {
                lua_pushstring(L, "user_id");
                lua_pushinteger(L, character_handle->user_id);
                lua_rawset(L, -3);

                lua_pushstring(L, "id");
                lua_pushinteger(L, entity.id().id());
                lua_rawset(L, -3);

                lua_pushstring(L, "type");
                lua_pushinteger(L, entity_types::character);
                lua_rawset(L, -3);
            }

            lua_setglobal(L, "roa_entity");

            if(!script.global) {
                uint64_t map_id;

                if(tile_handle) {
                    map_id = tile_handle->map_id;
                } else {
                    map_id = character_handle->map_id;
                }

                auto map_entity = get_map_entity_from_id(es, map_id);

                if(!map_entity) {
                    LOG(ERROR) << "non-global script without map entity " << map_id;
                    lua_close(L);
                    return;
                }

                auto map_handle = map_entity->component<map_component>();

                lua_newtable(L);

                lua_pushstring(L, "id");
                lua_pushinteger(L, map_entity->id().id());
                lua_rawset(L, -3);

                lua_pushstring(L, "first_tile_id");
                lua_pushinteger(L, map_handle->first_tile_id);
                lua_rawset(L, -3);

                lua_pushstring(L, "max_tile_id");
                lua_pushinteger(L, map_handle->max_tile_id);
                lua_rawset(L, -3);

                lua_pushstring(L, "width");
                lua_pushinteger(L, map_handle->width);
                lua_rawset(L, -3);

                lua_pushstring(L, "height");
                lua_pushinteger(L, map_handle->height);
                lua_rawset(L, -3);

                lua_setglobal(L, "roa_map");
            }

            auto result = lua_pcall(L, 0, LUA_MULTRET, 0);
            if (result) {
                LOG(ERROR) << NAMEOF(script_system::update) << " Failed to run script: " << lua_tostring(L, -1);
                lua_close(L);
                return;
            }
            lua_close(L);

            updated_scripts++;
            script.execute_in_ms = script.loop_every_ms - (dt - script.execute_in_ms);
        } else {
            script.execute_in_ms -= dt;
        }

        while(!_script_event_queue.empty()) {
            auto event = _script_event_queue.front();

            if(event->type == update_tile_event::type) {
                update_tile_event* update_event = dynamic_cast<update_tile_event*>(event.get());
                if(update_event != nullptr) {
                    entityx::Entity tile_entity = es.get(entityx::Entity::Id(update_event->id));
                    auto tile_handle = tile_entity.component<tile_component>();
                    LOG(DEBUG) << NAMEOF(script_system::update) << " updating tile " << tile_entity.id().id() << " map_id " << tile_handle->map_id << " new tile_id " << update_event->tile_id;
                    tile_handle->tile_id = update_event->tile_id;
                }
            }

            _script_event_queue.pop();
        }
    });
    auto end = chrono::high_resolution_clock::now();
    LOG(INFO) << NAMEOF(script_system::update) << " ran " << updated_scripts << " scripts in " << chrono::duration_cast<chrono::milliseconds>((end-start)).count() << " ms";
}
