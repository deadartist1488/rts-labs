# Лабораторная №6 «Корутины» (C++20)
# Имя для прогресс-бара (пункт 2) берётся из ../student.cmake

add_executable(lab6_1_example  example.cpp)
add_executable(lab6_2_progress progress.cpp)
foreach(t lab6_1_example lab6_2_progress)
  target_include_directories(${t} PRIVATE ${PROJECT_SOURCE_DIR})
  target_compile_features(${t} PRIVATE cxx_std_20)
endforeach()
target_compile_definitions(lab6_2_progress PRIVATE STUDENT_NAME="${STUDENT_NAME}")
