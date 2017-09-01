#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPUS="$( grep -c ^processor /proc/cpuinfo )"

cd $DIR/external/common_backend
./compile_external.sh
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .
make -j$CPUS

cd $DIR/external/entityx
# Release for entityx is -O2 -g -DNDEBUG
cmake -DCMAKE_BUILD_TYPE=Release -DENTITYX_DT_TYPE=uint32_t -DENTITYX_MAX_COMPONENTS=32
make -j$CPUS

cd $DIR/external/LuaJIT
make -j$CPUS
cd $DIR
