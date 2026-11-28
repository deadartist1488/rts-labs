# Лабораторная №X «По ком звонит Колокольчик!» (lock-free стек)

add_executable(lab8 main.cpp node.hpp lockfree_stack.hpp)
target_include_directories(lab8 PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(lab8 PRIVATE psapi)
