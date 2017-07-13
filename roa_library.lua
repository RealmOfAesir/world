--[[
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
--]]

local ffi = require("ffi")

ffi.cdef[[
void roa_log(int level, char const * message);
void set_tile_properties(uint64_t id, uint32_t tile_id);
]]

local enummt = {
    __index = function(table, key)
        if rawget(table.enums, key) then
            return key
        end
    end
}

local function Enum(t)
    local e = { enums = t }
    return setmetatable(e, enummt)
end

local SettingsClass = {}
SettingsClass.__index = SettingsClass

function SettingsClass:Create(debug, library_debug)
    local s = {}
    setmetatable(s, SettingsClass)
    s.debug = debug
    s.library_debug = library_debug
    return s
end

local MapClass = {}
MapClass.__index = MapClass

function MapClass:Create(id, first_tile_id, max_tile_id, width, height)
    local m = {}
    setmetatable(m, MapClass)
    m.id = id
    m.first_tile_id = first_tile_id
    m.max_tile_id = max_tile_id
    m.width = width
    m.height = height
    return m
end

function MapClass:get_id()
    return self.id
end

function MapClass:get_first_tile_id()
    return self.first_tile_id
end

function MapClass:get_max_tile_id()
    return self.max_tile_id
end

function MapClass:get_width()
    return self.width
end

function MapClass:get_height()
    return self.height
end

local TileClass = {}
TileClass.__index = TileClass

function TileClass:Create(id, tile_id, map)
    if map == nil then
        error("tile requires a map")
    end

    local t = {}
    setmetatable(t, TileClass)
    t.id = id
    t.tile_id = tile_id
    t.map = map
    return t
end

function TileClass:set_tile_id(id)
    if roa_settings["library_debug"] then
        ffi.C.roa_log(0, "setting tile id " .. self.id .. " to tile_id " .. id)
    end

    ffi.C.set_tile_properties(self.id, id)
    self.tile_id = id
end

function TileClass:get_tile_id()
    return self.tile_id
end

function TileClass:get_id()
    return self.id
end

function TileClass:get_map()
    return self.map
end

local roa = {}

-- cannot be enum, is given directly from C++ to lua through stack
roa.entity_types = {
    Tile = 1,
    Character = 2
}

roa.debug_level = Enum {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3
}

function roa.log(level, message)
    if level == roa.debug_level.Debug then
        ffi.C.roa_log(0, message)
    elseif level == roa.debug_level.Info then
        ffi.C.roa_log(1, message)
    elseif level == roa.debug_level.Warning then
        ffi.C.roa_log(2, message)
    elseif level == roa.debug_level.Error then
        ffi.C.roa_log(3, message)
    else
        ffi.C.roa_log(3, debug.traceback())
        error("enum type debug_level expected")
    end
end

function roa.get_triggering_entity()
    if(roa_entity == nil or roa_map == nil) then
        roa.log(roa.debug_level.Info, "Entity or map nil")
        return nil
    end

    if(roa_entity["type"] == roa.entity_types.Tile) then
        local map = MapClass:Create(roa_map["id"], roa_map["first_tile_id"], roa_map["max_tile_id"], roa_map["width"], roa_map["height"])
        return TileClass:Create(roa_entity["id"], roa_entity["tile_id"], map)
    elseif roa_settings["library_debug"] then
        roa.log(roa.debug_level.Debug, "type unknown")
    end

    return nil
end

function roa.get_settings()
    if(roa_settings == nil) then
        ffi.C.roa_log(3, debug.traceback())
        error("no settings available")
    end

    return SettingsClass:Create(roa_settings["debug"], roa_settings["library_debug"])
end

function roa.display_stack_parameters()
    roa.log(roa.debug_level.Debug, "settings: ")
    for k, v in pairs( roa_settings ) do
        roa.log(roa.debug_level.Debug, string.format("%s-%s", k, v))
    end

    roa.log(roa.debug_level.Debug, "entity: ")
    for k, v in pairs( roa_entity ) do
        roa.log(roa.debug_level.Debug, string.format("%s-%s", k, v))
    end

    roa.log(roa.debug_level.Debug, "map: ")
    for k, v in pairs( roa_map ) do
        roa.log(roa.debug_level.Debug, string.format("%s-%s", k, v))
    end
end

package.loaded["roa"] = roa