cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(Benchmark CXX)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

file(GLOB PROJECT_TEST_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/test_helpers/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/easyloggingpp/src/easylogging++.cc)
add_executable(Benchmark ${PROJECT_SOURCES} ${PROJECT_TEST_SOURCES} ${EASYLOGGING_SOURCE} main.cpp)

add_test(NAME MyBenchmark COMMAND Benchmark)

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src-cpp")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/easyloggingpp/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/cereal/include")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/di/include")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/libpqxx/include")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/json/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/Catch/include")
include_directories("${CMAKE_SOURCE_DIR}/external/entityx")
include_directories("${CMAKE_SOURCE_DIR}/external/LuaJIT/src")

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/libpqxx/src/.libs)
find_library(ENTITYX_LIBRARY entityx HINTS ${CMAKE_SOURCE_DIR}/external/entityx)
find_library(ROA_COMMON_LIBRARY RealmOfAesirCommon HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/common/bin)
find_library(ROA_COMMON_BACKEND_LIBRARY RealmOfAesirCommonBackend HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/bin)
find_library(LUAJIT_LIBRARY luajit HINTS ${CMAKE_SOURCE_DIR}/external/LuaJIT/src)

if(NOT RDKAFKA_LIBRARY)
    message(FATAL_ERROR "rdkafka not found")
endif()

if(NOT RDKAFKAPP_LIBRARY)
    message(FATAL_ERROR "rdkafkapp not found")
endif()

if(NOT PQXX_LIBRARY)
    message(FATAL_ERROR "pqxx not found")
endif()

if(NOT ROA_COMMON_LIBRARY)
    message(FATAL_ERROR "roa not found")
endif()

if(NOT ROA_COMMON_BACKEND_LIBRARY)
    message(FATAL_ERROR "roa_backend not found")
endif()

if(NOT ENTITYX_LIBRARY)
    message(FATAL_ERROR "ENTITYX_LIBRARY not found")
endif()

if(NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LUAJIT_LIBRARY not found")
endif()

target_link_libraries(Benchmark PUBLIC ${ROA_COMMON_LIBRARY})
target_link_libraries(Benchmark PUBLIC ${ROA_COMMON_BACKEND_LIBRARY})
target_link_libraries(Benchmark PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(Benchmark PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(Benchmark PUBLIC ${PQXX_LIBRARY})
target_link_libraries(Benchmark PUBLIC ${ENTITYX_LIBRARY})
target_link_libraries(Benchmark PUBLIC ${LUAJIT_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(Benchmark PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(Benchmark PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (Benchmark PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (Benchmark PUBLIC ${OPENSSL_LIBRARIES})
