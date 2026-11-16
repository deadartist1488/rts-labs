# Лабораторная №1 «Утренняя зарядка»
add_executable(lab1_1_warmup   warmup.cpp)
add_executable(lab1_2_factorial factorial.cpp)
add_executable(lab1_3_threads  threads.cpp)
foreach(t lab1_1_warmup lab1_2_factorial lab1_3_threads)
  target_include_directories(${t} PRIVATE ${PROJECT_SOURCE_DIR})
endforeach()
