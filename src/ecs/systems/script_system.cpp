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

using namespace std;
using namespace roa;

queue<shared_ptr<event_type>> _script_event_queue;

void script_system::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) {
    int updated_scripts = 0;
    es.each<script_component>([&](entityx::Entity entity, script_component& script) {
        if(script.execute_in_ms <= _config.tick_length) {

            auto tile_handle = entity.component<tile_component>();
            auto character_handle = entity.component<character_component>();

            LOG(INFO) << NAMEOF(script_system::update) << " setting up script";

            auto L = load_script_with_libraries(script.script_text);

            lua_newtable(L);

            lua_pushstring(L, "debug");
            lua_pushboolean(L, false);
            lua_rawset(L, -3);

            lua_setglobal(L, "settings");

            lua_newtable(L);

            lua_pushstring(L, "tile_id");
            lua_pushinteger(L, tile_handle->tile_id);
            lua_rawset(L, -3);

            lua_pushstring(L, "id");
            lua_pushinteger(L, tile_handle->id);
            lua_rawset(L, -3);

            lua_pushstring(L, "type");
            lua_pushinteger(L, 1);
            lua_rawset(L, -3);

            lua_setglobal(L, "entity");

            lua_newtable(L);

            lua_pushstring(L, "first_tile_id");
            lua_pushinteger(L, 1);
            lua_rawset(L, -3);

            lua_pushstring(L, "max_tile_id");
            lua_pushinteger(L, 10);
            lua_rawset(L, -3);

            lua_setglobal(L, "map");

            auto result = lua_pcall(L, 0, LUA_MULTRET, 0);
            if (result) {
                LOG(ERROR) << NAMEOF(script_system::update) << " Failed to run script: " << lua_tostring(L, -1);
                lua_close(L);
                return;
            }
            lua_close(L);

            updated_scripts++;
            script.execute_in_ms = script.loop_every_ms - (_config.tick_length - script.execute_in_ms);
        } else {
            script.execute_in_ms -= _config.tick_length;
        }

        while(!_script_event_queue.empty()) {
            auto event = _script_event_queue.front();

            if(event->type == update_tile_event::type) {
                update_tile_event* update_event = static_cast<update_tile_event*>(event.get());
                if(update_event != nullptr) {
                    entityx::ComponentHandle<tile_component> tile;
                    for(entityx::Entity entity : es.entities_with_components(tile)) {
                        if(tile->id == update_event->id) {
                            tile->tile_id = update_event->tile_id;
                            break;
                        }
                    };
                }
            }

            _script_event_queue.pop();
        }
    });
    LOG(INFO) << NAMEOF(script_system::update) << " ran " << updated_scripts << " scripts";
}
