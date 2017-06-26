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

namespace roa {
    struct tileset {
        tileset() = default;
        tileset(uint32_t first_gid, std::string image_path, uint32_t tile_width, uint32_t tile_height,
                uint32_t width, uint32_t height) : first_gid(first_gid), image_path(image_path), tile_width(tile_width),
                                                   tile_height(tile_height), width(width), height(height) {}

        uint32_t const first_gid;
        std::string const image_path;
        uint32_t const tile_width;
        uint32_t const tile_height;
        uint32_t const width;
        uint32_t const height;
    };

    struct map_component {
        map_component(uint32_t id, uint32_t tile_height, uint32_t tile_width,
                      uint32_t width, uint32_t height, uint32_t layers)
                : id(id), tile_height(tile_height), tile_width(tile_width),
                  width(width), height(height), layers(layers)
        {}

        uint32_t const id;
        uint32_t const tile_height;
        uint32_t const tile_width;
        uint32_t const width;
        uint32_t const height;
        uint32_t const layers;
        std::vector<tileset> tilesets;
        std::vector<std::vector<std::vector<entityx::Entity>>> tiles;
        std::vector<entityx::Entity> npcs;
        std::vector<entityx::Entity> pcs;
        std::vector<entityx::Entity> items;
    };
}