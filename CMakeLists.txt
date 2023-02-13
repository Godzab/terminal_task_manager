cmake_minimum_required(VERSION 3.22)
project(TaskManagerTerminal)

find_package(Curses REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(${CURSES_INCLUDE_DIRS})
include_directories(${SQLite3_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

file(GLOB project_SRCS src/*.cpp) #src/*.h

add_executable(TaskManagerTerminal ${project_SRCS})

target_link_libraries(TaskManagerTerminal ${CURSES_LIBRARIES} ${SQLite3_LIBRARIES})

target_compile_options(TaskManagerTerminal PRIVATE -Wall -Wextra)
