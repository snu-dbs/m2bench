cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)

project(m2bench_ao)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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

# m2bench_ao
include_directories(include)
include_directories(src)

add_executable(m2bench_ao main.cpp
        include/Cursor.h
        include/Connection/Connection.h
        include/Connection/OrientConnection.h
        include/Connection/ArangoConnection.h
        include/Connection/ScidbConnection.h
        include/Connection/ScidbPrimitives.h
        include/Session/Session.h
        include/Session/ArangoSession.h
        include/Session/OrientSession.h
        include/Session/ScidbSession.h

        include/Connection/mysql_connector.h
        include/Connection/neo4j_connector.h
        include/Connection/mongodb_connector.h

        src/Connection/mysql_connector.cpp
        src/Connection/neo4j_connector.cpp
        src/Connection/mongodb_connector.cpp

        src/Example/mysql_example.h
        src/Example/neo4j_example.h
        src/Example/mongodb_example.h
        src/Example/scidb.h
        src/Example/arangodb.h
        src/Example/orientdb.h

        src/Scenarios/PolyglotDB/Ecommerce_Tasks.h
        src/Scenarios/PolyglotDB/Healthcare_Tasks.h
        src/Scenarios/PolyglotDB/Disaster_Tasks.h

        src/Cursor.cpp
        src/Connection/ArangoConnection.cpp
        src/Session/ArangoSession.cpp
        src/Connection/OrientConnection.cpp
        src/Session/OrientSession.cpp
        src/Session/ScidbSession.cpp
        src/Connection/ScidbConnection.cpp

        src/CurlHelper.cpp
        include/CurlHelper.h
        src/Scenarios/AgensGraph/Ecommerce_Tasks.h src/Scenarios/AgensGraph/Healthcare_Tasks.h src/Scenarios/AgensGraph/Disaster_Tasks.h src/Generator/DataScaler.cpp src/Generator/DataScaler.h)

target_link_libraries(m2bench_ao PRIVATE cpr::cpr)
target_link_libraries(m2bench_ao PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(m2bench_ao PUBLIC /usr/local/lib/libmongocxx.so /usr/local/lib/libbsoncxx.so)
target_link_libraries(m2bench_ao PUBLIC -lneo4j-client)
target_link_libraries(m2bench_ao PUBLIC -lmysqlcppconn8)
