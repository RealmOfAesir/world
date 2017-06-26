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

#include <lua.hpp>
#include <iostream>

namespace roa {
    lua_State* load_script_with_libraries(std::string script);
    void set_library_script(std::string script);

    class event_type {
    public:
        explicit event_type(uint32_t type) : type(type) {}
        uint32_t type;
    };

    class update_tile_event : public event_type {
    public:
        update_tile_event() = default;
        update_tile_event(uint32_t id, uint32_t tile_id) : event_type(update_tile_event::type), id(id), tile_id(tile_id) {}

        static constexpr uint32_t type = 1;
        uint32_t id;
        uint32_t tile_id;
    };
}