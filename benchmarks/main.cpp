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

#include <easylogging++.h>
#include <lua/lua_interop.h>
#include <chrono>
#include <macros.h>

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace roa;

void benchmark_loading_and_running_lua()
{
    std::string lib;
    std::string src_file;

    {
        std::ifstream lib_stream("test_library.lua");
        lib_stream.seekg(0, std::ios::end);
        lib.reserve(lib_stream.tellg());
        lib_stream.seekg(0, std::ios::beg);

        lib.assign((std::istreambuf_iterator<char>(lib_stream)),
                   std::istreambuf_iterator<char>());
    }

    {
        std::ifstream src_file_stream("test.lua");
        src_file_stream.seekg(0, std::ios::end);
        src_file.reserve(src_file_stream.tellg());
        src_file_stream.seekg(0, std::ios::beg);

        src_file.assign((std::istreambuf_iterator<char>(src_file_stream)),
                        std::istreambuf_iterator<char>());
    }

    set_library_script(lib);
    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < 5000; i++) {
        auto L = load_script_with_libraries(src_file);

        lua_newtable(L);

        lua_pushstring(L, "debug");
        lua_pushboolean(L, true);
        lua_rawset(L, -3);

        lua_pushstring(L, "library_debug");
        lua_pushboolean(L, false);
        lua_rawset(L, -3);

        lua_setglobal(L, "roa_settings");

        lua_newtable(L);

        lua_pushstring(L, "tile_id");
        lua_pushinteger(L, 1);
        lua_rawset(L, -3);

        lua_pushstring(L, "id");
        lua_pushinteger(L, 1);
        lua_rawset(L, -3);

        lua_pushstring(L, "type");
        lua_pushinteger(L, 1);
        lua_rawset(L, -3);

        lua_setglobal(L, "roa_entity");

        lua_newtable(L);

        lua_pushstring(L, "id");
        lua_pushinteger(L, 2);
        lua_rawset(L, -3);

        lua_pushstring(L, "first_tile_id");
        lua_pushinteger(L, 2);
        lua_rawset(L, -3);

        lua_pushstring(L, "max_tile_id");
        lua_pushinteger(L, 2);
        lua_rawset(L, -3);

        lua_pushstring(L, "width");
        lua_pushinteger(L, 2);
        lua_rawset(L, -3);

        lua_pushstring(L, "height");
        lua_pushinteger(L, 2);
        lua_rawset(L, -3);

        lua_setglobal(L, "roa_map");

        auto result = lua_pcall(L, 0, LUA_MULTRET, 0);
        if (result) {
            LOG(ERROR) << " Failed to run script: " << lua_tostring(L, -1);
            lua_close(L);
            throw runtime_error("Failed to run script");
        }
        lua_close(L);
    }
    auto end = chrono::high_resolution_clock::now();

    LOG(INFO) << NAMEOF(benchmark_loading_and_running_lua) << " time required to run: " << chrono::duration_cast<chrono::milliseconds>((end-start)).count() << " ms";
}

void init_stuff() {
    ios::sync_with_stdio(false);

    el::Configurations defaultConf;
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level: %msg");
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

int main(int argc, char const * const * argv) {
    init_stuff();

    benchmark_loading_and_running_lua();

    return 0;
}