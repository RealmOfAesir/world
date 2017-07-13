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

#include <queue>
#include "lua_interop.h"
#include "easylogging++.h"

using namespace std;
using namespace roa;

extern queue<shared_ptr<event_type>> _script_event_queue;

extern "C" void roa_log(int level, char const *message) {
    switch (level) {
        case 0:
            LOG(DEBUG) << message;
            break;
        case 1:
            LOG(INFO) << message;
            break;
        case 2:
            LOG(WARNING) << message;
            break;
        default:
            LOG(ERROR) << message;
            break;
    }
}

extern "C" void set_tile_properties(uint64_t id, uint32_t tile_id) {
    _script_event_queue.emplace<shared_ptr<update_tile_event>>(make_shared<update_tile_event>(id, tile_id));
}

