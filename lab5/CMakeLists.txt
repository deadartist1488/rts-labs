# Лабораторная №5 «Самодельный мьютекс»

add_executable(lab5 main.cpp spin_mutex.h)
target_include_directories(lab5 PRIVATE ${PROJECT_SOURCE_DIR})

# Зависимость от лаб.4: библиотека fatties объявлена в lab4/CMakeLists.txt.
# Она подключает lab4/fatties.h (шаблон Table<Mutex>), в который здесь
# подставляется SpinMutex вместо std::mutex.
target_link_libraries(lab5 PRIVATE fatties)
