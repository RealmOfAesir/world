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

#include <catch.hpp>
#include <easylogging++.h>
#include <ecs/ecs.h>
#include <helpers/tiled_converter.h>

using namespace std;
using namespace roa;

TEST_CASE("convert_map_to_json tests") {
    EntityManager _ex;
    auto map_entity = _ex.create();
    auto& mc = _ex.assign<map_component>(map_entity, 0u, 64u, 64u, 640u, 640u, 1u, 1u, 101u);
    mc.tilesets.emplace_back(1, "terrain.png"s, 64, 64, 1536, 2560);
    mc.layers.emplace_back(vector<uint64_t>{}, 0, 0, 500, 500);

    mc.layers[0].tiles.reserve(500*500);

    for(uint32_t x = 0; x < 500; x++) {
        for(uint32_t y = 0; y < 500; y++) {
            auto tile_entity = _ex.create();
            _ex.assign<tile_component>(tile_entity, map_entity, y+1);
            mc.layers[0].tiles.push_back(tile_entity);
        }
    }

    auto start = chrono::high_resolution_clock::now();
    string ret;
    for(int i = 0; i < 100; i++) {
        CHECK_NOTHROW(ret = tiled_converter::convert_map_to_json(_ex, mc));
    }
    auto end = chrono::high_resolution_clock::now();

    LOG(INFO) << " time required to run: " << chrono::duration_cast<chrono::milliseconds>((end - start)).count() << " ms";
    LOG(INFO) << ret;

    _ex.reset();
}