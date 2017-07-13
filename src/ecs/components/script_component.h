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

namespace roa {
    enum trigger_type_enum {
        once,
        looped,
        chat,
        movement
    };

    struct script_component {
        script_component(std::string name, std::string script_text, uint32_t execute_in_ms, uint32_t loop_every_ms,
                         trigger_type_enum trigger_type, bool debug)
                : name(name), script_text(script_text), times_executed(0), execute_in_ms(execute_in_ms),
                  loop_every_ms(loop_every_ms), trigger_type(trigger_type), debug(debug) {}

        std::string name;
        std::string script_text;
        uint32_t times_executed;
        uint32_t execute_in_ms;
        uint32_t loop_every_ms;
        trigger_type_enum trigger_type;
        bool global;
        bool debug;
    };
}