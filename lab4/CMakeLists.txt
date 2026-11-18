# Лабораторная №4 «Три толстяка»

# Модель толстяков (fatties.h) оформлена как отдельная header-only библиотека,
# чтобы её можно было подключить в лаб.5 через target_link_libraries.
add_library(fatties INTERFACE)
target_sources(fatties INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fatties.h)
target_include_directories(fatties INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(lab4 main.cpp)
target_include_directories(lab4 PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(lab4 PRIVATE fatties)
