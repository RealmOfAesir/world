/*
    RealmOfAesirWorld
    Copyright (C) 2017  Michael de Lang

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

#include <easylogging++.h>
#include <macros.h>
#include <messages/error_response_message.h>
#include <events/player_events/enter_map_event.h>
#include "player_event_system.h"
#include <cereal/archives/json.hpp>

using namespace std;
using namespace roa;
using namespace rapidjson;

void player_event_system::update(EntityManager &es, TimeDelta dt) {
    LOG(TRACE) << NAMEOF(player_event_system::update) << " starting tick";

    shared_ptr<event_type> event;
    while(_player_event_queue.try_dequeue(event)) {
        if(event->type == enter_world_event::type) {
            auto enter_event = dynamic_cast<enter_world_event const * const>(event.get());

            if(!enter_event) {
                LOG(ERROR) << NAMEOF(world::do_tick) << " couldn't cast enter_world_event";
                continue;
            }

            if(auto map = get_map(es, enter_event->_player)) {

            } else {
                send_error(enter_event);
            }
        }
    }

    LOG(TRACE) << NAMEOF(player_event_system::update) << " ending tick";
}

STD_OPTIONAL<map_component> player_event_system::get_map(EntityManager &es, player const &plyr) const noexcept {
    auto map_id = plyr.location->map_id;
    for(auto map_entity : es.view<map_component>()) {
        auto& map_comp = es.get<map_component>(map_entity);
        if(map_comp.id == map_id) {
            return map_comp;
        }
    }

    return {};
}

void player_event_system::send_error(player_event const * const event) const noexcept {
    LOG(ERROR) << NAMEOF(player_event_system::do_tick) << " couldn't execute action";

    string queue_name = "server-" + to_string(event->_player_gateway_id);
    _producer->enqueue_message(queue_name, binary_error_response_message {
            {false, event->_player_client_id, _config.server_id, 0 /* ANY */},
            -1,
            "Couldn't execute action"
    });
}