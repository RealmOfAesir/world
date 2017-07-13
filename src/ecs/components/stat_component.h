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
    struct stat_component {
        stat_component(uint64_t user_id) : user_id(user_id) {}

        std::string name;
        uint64_t static_value;
        uint32_t dice;
        uint32_t die_face;
        player_id BIGINT NOT NULL,
                stat_id INT NOT NULL,
        is_growth BOOLEAN NOT NULL,
                static_value BIGINT NOT NULL,
        dice INT NULL,
        die_face INT NULL
    };
}