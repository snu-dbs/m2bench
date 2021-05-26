cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)

project(m2bench_ao)

include(FetchContent)

# cpr (curl lib)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9) # the commit hash for 1.5.0
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

        src/Cursor.cpp
        src/Connection/ArangoConnection.cpp
        src/Session/ArangoSession.cpp
        src/Connection/OrientConnection.cpp
        src/Session/OrientSession.cpp
        src/Session/ScidbSession.cpp
        src/Connection/ScidbConnection.cpp

        src/CurlHelper.cpp
        include/CurlHelper.h )

target_link_libraries(m2bench_ao PRIVATE cpr::cpr)
target_link_libraries(m2bench_ao PRIVATE nlohmann_json::nlohmann_json)
