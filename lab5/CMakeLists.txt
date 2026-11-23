# Лабораторная №5 «Самодельный мьютекс»

add_executable(lab5 main.cpp spin_mutex.h)
target_include_directories(lab5 PRIVATE ${PROJECT_SOURCE_DIR})
