cmake_minimum_required(VERSION 3.20)
project(rts_labs LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/student.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  # исходники в UTF-8 (русский текст в выводе)
  add_compile_options(/utf-8 /W4)
endif()

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab3)
add_subdirectory(lab4)
add_subdirectory(lab5)
add_subdirectory(lab6)
add_subdirectory(lab7)
