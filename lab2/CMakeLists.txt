# Лабораторная №2 «What's going on?»
add_executable(lab2 main.cpp)
target_include_directories(lab2 PRIVATE ${PROJECT_SOURCE_DIR})

# Тот же код, но без мьютекса m (последний пункт задания)
add_executable(lab2_no_mutex main.cpp)
target_include_directories(lab2_no_mutex PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_definitions(lab2_no_mutex PRIVATE NO_MUTEX)
