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
#include <catch.hpp>
#include <ecs/ecs.h>
#include <helpers/tiled_converter.h>
#include <json.hpp>
#include <base64.h>
#include <lz4.h>

using namespace std;
using namespace roa;
using namespace nlohmann;

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
        REQUIRE_NOTHROW(ret = tiled_converter::convert_map_to_json(_ex, mc));
    }
    auto end = chrono::high_resolution_clock::now();

    LOG(INFO) << " time required to run: " << chrono::duration_cast<chrono::milliseconds>((end - start)).count() << " ms";
    LOG(INFO) << ret;

    _ex.reset();

    auto json_map = json::parse(ret);
    std::string base64data = json_map["layers"][0]["data"];
    std::string data = base64_decode(base64data);

    int tiles_size = 500 * 500 * 4;
    char* tiles = new char[tiles_size]();

    auto lz4_ret = LZ4_decompress_safe(data.c_str(), tiles, data.size(), tiles_size);
    CHECK(lz4_ret > 0);

    if(lz4_ret > 0) {
        for (uint32_t x = 0; x < 500; x++) {
            for (uint32_t y = 0; y < 500; y++) {
                uint32_t tocheck = reinterpret_cast<uint32_t *>(tiles)[x * 500 + y];
                CHECK(tocheck == y + 1);
            }
        }
    }

    delete tiles;
}