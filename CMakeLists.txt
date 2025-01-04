cmake_minimum_required(VERSION 3.20)

project(aoc_year2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(base INTERFACE)
target_compile_options(base INTERFACE -Wall -Wextra -Wpedantic -Wnon-virtual-dtor)
if(ENABLE_ADDRESS_SANITIZER)
  target_compile_options(base INTERFACE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(base INTERFACE -fsanitize=address)
endif()

set(days day01 day02 day03 day04 day05 day06 day07 day08 day09 day10 day11 day12 day13 day14 day15 day16 day17 day18 day19 day20 day22 day23 day24 day25)

foreach(day ${days})
  add_executable(${day} ${day}/main.cpp)
  set_property(TARGET ${day} PROPERTY OUTPUT_NAME main)
  set_property(TARGET ${day} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${day})
  target_link_libraries(${day} base)

  configure_file(${day}/input.txt ${day}/input.txt COPYONLY)
endforeach()
