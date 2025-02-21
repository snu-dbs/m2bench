cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(m2bench)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# MongoDB driver
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

# MySQL driver
find_package(mysql-concpp REQUIRED)

include(FetchContent)

# cpr (curl lib)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG 1.9.9)
FetchContent_MakeAvailable(cpr)

# json
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# m2bench
include_directories(include src)

add_executable(m2bench main.cpp
    src/Connection/mysql_connector.cpp
    src/Connection/mongodb_connector.cpp

    src/Polyglot/Ecommerce_Tasks.cpp
    src/Polyglot/Healthcare_Tasks.cpp
    src/Polyglot/Disaster_Tasks.cpp

    src/Cursor.cpp
    src/Session/ScidbSession.cpp
    src/Connection/ScidbConnection.cpp
    src/CurlHelper.cpp
)

target_link_libraries(m2bench PRIVATE cpr::cpr)
target_link_libraries(m2bench PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(m2bench PRIVATE mongocxx bsoncxx)
target_link_libraries(m2bench PRIVATE mongo::mongocxx_shared mongo::bsoncxx_shared)
target_link_libraries(m2bench PUBLIC mysql::concpp)
