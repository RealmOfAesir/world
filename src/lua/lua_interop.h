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
#include <ecs/components/script_component.h>
#include "lua_script.h"
#include <memory>

#define USE_LOCAL_FILES 1

namespace roa {
    class iscripts_repository;
    class script;

    lua_script load_script_with_libraries(std::string name, std::string script);
    void set_library_script(std::string script);
    void set_scripts_repository(std::shared_ptr<iscripts_repository> scripts_repo);
    std::shared_ptr<script> load_script(std::string name);

    class event_type {
    public:
        explicit event_type(uint32_t type) : type(type) {}
        virtual ~event_type() {};
        uint32_t type;
    };

    class update_tile_event : public event_type {
    public:
        update_tile_event() = default;
        update_tile_event(uint64_t id, uint32_t tile_id) : event_type(update_tile_event::type), id(id), tile_id(tile_id) {}
        virtual ~update_tile_event() {}

        static constexpr uint32_t type = 1;
        uint64_t id;
        uint32_t tile_id;
    };

    class destroy_script_event : public event_type {
    public:
        destroy_script_event() = default;
        destroy_script_event(uint64_t id) : event_type(destroy_script_event::type), id(id) {}
        virtual ~destroy_script_event() {}

        static constexpr uint32_t type = 2;
        uint64_t id;
    };

    class create_script_event : public event_type {
    public:
        create_script_event() = default;
        create_script_event(std::string name, std::string script, uint64_t entity_id, uint32_t execute_in_ms, uint32_t loop_every_ms, trigger_type_enum trigger_type, bool debug)
                : event_type(create_script_event::type), name(name), script(script), entity_id(entity_id), execute_in_ms(execute_in_ms), loop_every_ms(loop_every_ms), trigger_type(trigger_type), debug(debug) {}
        virtual ~create_script_event() {}

        static constexpr uint32_t type = 3;
        std::string name;
        std::string script;
        uint64_t entity_id;
        uint32_t execute_in_ms;
        uint32_t loop_every_ms;
        trigger_type_enum trigger_type;
        bool debug;
    };
}