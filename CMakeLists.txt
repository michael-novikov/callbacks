cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Release)

set(this_project callbacks)

project(${this_project} CXX)

set(headers
  )

set(sources
  test.cpp
  )

add_executable(${this_project} ${sources} ${headers})

