# Лабораторная №5 «Самодельный мьютекс»
# Берёт модель толстяков из lab4/fatties.h и подставляет свой мьютекс.
add_executable(lab5 main.cpp spin_mutex.h)
target_include_directories(lab5 PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/lab4)
