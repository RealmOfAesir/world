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

#include <macros.h>
#include "lua_interop.h"
#include "easylogging++.h"

using namespace std;

namespace roa {
    string library_script;

    lua_script load_script_with_libraries(std::string script) {
        return lua_script(library_script, script);
    }

    void set_library_script(std::string script) {
        library_script = script;
    }
}