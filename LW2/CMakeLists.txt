set(CMAKE_C_STANDARD 11)
project(mai-os)

# Создание библиотеки из utils.c
add_library(LW2_utils src/utils.c)
target_include_directories(LW2_utils PRIVATE include)

# Создание исполнимого файла из main.c
add_executable(lab2 main.c)

# Включение директории с заголовочными файлами
target_include_directories(lab2 PRIVATE include)

# Линковка с библиотекой utils и математической библиотекой (m)
target_link_libraries(lab2 LW2_utils m)  # Добавляем m для линковки с математической библиотекой
