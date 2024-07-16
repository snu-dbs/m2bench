cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)

project(m2bench)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Boost
find_package (Boost 1.70.0 REQUIRED COMPONENTS unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})
find_library(Boost_LIBRARIES boost ${Boost_LIBRARIES_DIR})


#mongoDB driver
include_directories ("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/libmongoc-1.0")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/libbson-1.0")
include_directories("/usr/local/lib -lmongocxx -lbsoncxx")

#mysql driver
include_directories("/usr/include/mysql-cppconn-8")


include(FetchContent)

# cpr (curl lib)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG aac5058a15e9ad5ad393973dc6fe44d7614a7f55) # the commit hash for 1.5.0
FetchContent_MakeAvailable(cpr)

# json
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# m2bench
include_directories(include)
include_directories(src)

add_executable(m2bench main.cpp
        src/Connection/mysql_connector.cpp
        src/Connection/neo4j_connector.cpp
        src/Connection/mongodb_connector.cpp

        src/Polyglot/Ecommerce_Tasks.cpp
        src/Polyglot/Healthcare_Tasks.cpp
        src/Polyglot/Disaster_Tasks.cpp
        src/Polyglot/Latency_Check.cpp

        src/Cursor.cpp
        src/Session/ScidbSession.cpp
        src/Connection/ScidbConnection.cpp

        src/CurlHelper.cpp
        include/CurlHelper.h
        )

target_link_libraries(m2bench PRIVATE cpr::cpr)
target_link_libraries(m2bench PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(m2bench PUBLIC -lmongocxx)
target_link_libraries(m2bench PUBLIC -lbsoncxx)
target_link_libraries(m2bench PUBLIC -lneo4j-client)
target_link_libraries(m2bench PUBLIC -lmysqlcppconn8)
target_link_libraries(m2bench PUBLIC ${Boost_LIBRARIES})
