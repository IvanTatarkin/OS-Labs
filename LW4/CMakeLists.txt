cmake_minimum_required(VERSION 3.10)

project(lab4)

set(CMAKE_C_STANDARD 17)

# Создаём библиотеки
add_library(lib1 SHARED src/lib1.c)
add_library(lib2 SHARED src/lib2.c)

# Линкуем math для библиотек
target_link_libraries(lib1 PRIVATE m)
target_link_libraries(lib2 PRIVATE m)

# Создаём исполняемый файл для статической программы
add_executable(static main_static.c)
target_link_libraries(static PRIVATE lib1 m)

# Создаём исполняемый файл для динамической программы
add_executable(dynamic main_dynamic.c)
target_link_libraries(dynamic PRIVATE dl)

