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

#include "lua_interop.h"
#include "easylogging++.h"

using namespace std;

namespace roa {
    string library_script;

    lua_State *load_script_with_libraries(string script) {
        int status;
        lua_State *L;

        L = luaL_newstate();

        luaL_openlibs(L);

        status = luaL_loadstring(L, library_script.c_str());
        if (status) {
            LOG(ERROR) << "Couldn't load library script: " << lua_tostring(L, -1);
            lua_close(L);
            throw runtime_error("Couldn't load library script");
        }

        status = lua_pcall(L, 0, LUA_MULTRET, 0);
        if (status) {
            LOG(ERROR) << "Couldn't run library script: " << lua_tostring(L, -1);
            lua_close(L);
            throw runtime_error("Couldn't run library script");
        }

        status = luaL_loadstring(L, script.c_str());
        if (status) {
            LOG(ERROR) << "Couldn't load script: " << lua_tostring(L, -1);
            lua_close(L);
            throw runtime_error("Couldn't load script");
        }

        return L;
    }

    void set_library_script(std::string script) {
        library_script = script;
    }
}