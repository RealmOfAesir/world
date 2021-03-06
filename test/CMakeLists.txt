cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(Test CXX)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

file(GLOB PROJECT_TEST_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/test_helpers/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/easyloggingpp/src/easylogging++.cc)
add_executable(Test ${PROJECT_SOURCES} ${PROJECT_TEST_SOURCES} ${EASYLOGGING_SOURCE} main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_LOG_TO_FILE ")

add_test(NAME MyTest COMMAND Test)

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
include_directories("${CMAKE_SOURCE_DIR}/external/entt/src")
include_directories("${CMAKE_SOURCE_DIR}/external/readerwriterqueue")
include_directories("${CMAKE_SOURCE_DIR}/external/LuaJIT/src")
include_directories("${CMAKE_SOURCE_DIR}/external/lz4/lib")

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/libpqxx/src/.libs)
find_library(ROA_COMMON_LIBRARY RealmOfAesirCommon HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/common/bin)
find_library(ROA_COMMON_BACKEND_LIBRARY RealmOfAesirCommonBackend HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/bin)
find_library(LUAJIT_LIBRARY luajit HINTS ${CMAKE_SOURCE_DIR}/external/LuaJIT/src)
find_library(LZ_LIBRARY lz4 HINTS ${CMAKE_SOURCE_DIR}/external/lz4/lib)

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

if(NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LUAJIT_LIBRARY not found")
endif()

if(NOT LZ_LIBRARY)
    message(FATAL_ERROR "LZ_LIBRARY not found")
endif()

target_link_libraries(Test PUBLIC ${ROA_COMMON_LIBRARY})
target_link_libraries(Test PUBLIC ${ROA_COMMON_BACKEND_LIBRARY})
target_link_libraries(Test PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(Test PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(Test PUBLIC ${PQXX_LIBRARY})
target_link_libraries(Test PUBLIC ${LUAJIT_LIBRARY})
target_link_libraries(Test PUBLIC ${LZ_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(Test PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(Test PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (Test PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (Test PUBLIC ${OPENSSL_LIBRARIES})
