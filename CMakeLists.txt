cmake_minimum_required(VERSION 3.28)
project(chess VERSION 0.1.0)

set(BUILD_MODE "library" CACHE STRING "Build lib file")

add_subdirectory(engine)
add_subdirectory(ui)
