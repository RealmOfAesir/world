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

#include "../../lua/lua_script.h"

namespace roa {
    enum trigger_type_enum {
        once,
        looped,
        chat,
        movement
    };

    struct script_component {
        script_component(std::string name, lua_script script, uint32_t execute_in_ms, uint32_t loop_every_ms,
                         trigger_type_enum trigger_type, bool debug)
                : name(name), script(std::move(script)), times_executed(0), execute_in_ms(execute_in_ms),
                  loop_every_ms(loop_every_ms), trigger_type(trigger_type), debug(debug) {}
        /*script_component(script_component const &sc) = delete;
        script_component(script_component &&sc) noexcept
                : name(sc.name), script(std::move(sc.script)), times_executed(sc.times_executed),
                  execute_in_ms(sc.execute_in_ms), loop_every_ms(sc.loop_every_ms), trigger_type(sc.trigger_type),
                  global(sc.global), debug(sc.debug) {}*/

        std::string name;
        lua_script script;
        uint32_t times_executed;
        uint32_t execute_in_ms;
        uint32_t loop_every_ms;
        trigger_type_enum trigger_type;
        bool global;
        bool debug;
    };
}