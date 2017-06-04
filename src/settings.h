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

#include <string>

/*INSERT INTO settings ("name", value) VALUES ('xp_gain', '1');
INSERT INTO settings ("name", value) VALUES ('money_gain', '1');
INSERT INTO settings ("name", value) VALUES ('movement_multiplier', '1');
INSERT INTO settings ("name", value) VALUES ('drop_rate_multiplier', '1');
INSERT INTO settings ("name", value) VALUES ('maintenance_mode', '0');
INSERT INTO settings ("name", value) VALUES ('player_start_script_zone', '0');*/

namespace roa {
    struct Settings {
        double xp_gain;
        double money_gain;
        double movement_multiplier;
        double drop_rate_multiplier;
        bool maintenance_mode;
        std::string player_start_script_zone;
    };
}