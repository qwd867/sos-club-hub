cmake_minimum_required(VERSION 3.10)
project(AuthServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_executable(auth_server
    src/main.cpp
    src/user.cpp
    src/sha256.cpp
    src/http_response.cpp
    src/database_manager.cpp
    src/auth_service.cpp
    src/router.cpp
    src/server.cpp
    third_party/sqlite3.c
)

target_link_libraries(auth_server pthread dl)
