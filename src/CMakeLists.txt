cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(RealmOfAesirWorld CXX)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB EASYLOGGING_SOURCE ${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/easyloggingpp/src/easylogging++.cc)
add_executable(RealmOfAesirWorld ${EASYLOGGING_SOURCE} ${PROJECT_SOURCES} )

include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src-cpp")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/easyloggingpp/src")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/cereal/include")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/di/include")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/libpqxx/include")
include_directories("${CMAKE_SOURCE_DIR}/external/common_backend/external/common/external/json/src")
include_directories("${CMAKE_SOURCE_DIR}/external/entt/src")
include_directories("${CMAKE_SOURCE_DIR}/external/LuaJIT/src")

find_library(RDKAFKA_LIBRARY rdkafka HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src)
find_library(RDKAFKAPP_LIBRARY rdkafka++ HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/librdkafka/src-cpp)
find_library(PQXX_LIBRARY pqxx HINTS ${CMAKE_SOURCE_DIR}/external/common_backend/external/libpqxx/src/.libs)
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
    message(FATAL_ERROR "libpqxx not found")
endif()

if(NOT ROA_COMMON_LIBRARY)
    message(FATAL_ERROR "libroa not found")
endif()

if(NOT ROA_COMMON_BACKEND_LIBRARY)
    message(FATAL_ERROR "ROA_backend not found")
endif()

if(NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LUAJIT_LIBRARY not found")
endif()

target_link_libraries(RealmOfAesirWorld PUBLIC ${ROA_COMMON_BACKEND_LIBRARY})
target_link_libraries(RealmOfAesirWorld PUBLIC ${ROA_COMMON_LIBRARY})
target_link_libraries(RealmOfAesirWorld PUBLIC ${RDKAFKAPP_LIBRARY})
target_link_libraries(RealmOfAesirWorld PUBLIC ${RDKAFKA_LIBRARY})
target_link_libraries(RealmOfAesirWorld PUBLIC ${PQXX_LIBRARY})
target_link_libraries(RealmOfAesirWorld PUBLIC ${LUAJIT_LIBRARY})

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(RealmOfAesirWorld PUBLIC ${PostgreSQL_LIBRARIES})

find_package( ZLIB REQUIRED )
include_directories( ${ZLIB_INCLUDE_DIRS} )
target_link_libraries(RealmOfAesirWorld PUBLIC ${ZLIB_LIBRARIES} )

find_package (Threads)
target_link_libraries (RealmOfAesirWorld PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL REQUIRED)
target_link_libraries (RealmOfAesirWorld PUBLIC ${OPENSSL_LIBRARIES})
