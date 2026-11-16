# Лабораторная №4 «Три толстяка»
add_executable(lab4 main.cpp fatties.h)
target_include_directories(lab4 PRIVATE ${PROJECT_SOURCE_DIR})
