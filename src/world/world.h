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

#include <entityx/entityx.h>
#include <vector>
#include <database_pool.h>

namespace roa {
    class world {
    public:

        void do_tick(uint32_t tick_length);
        void load_from_database(std::shared_ptr<idatabase_pool> db_pool, Config& config);

    private:
        entityx::EntityX _ex;
        std::vector<entityx::Entity> _maps;
    };
}