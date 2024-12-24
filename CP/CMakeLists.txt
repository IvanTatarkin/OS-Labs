cmake_minimum_required(VERSION 3.10)
project(Battleship)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем директорию include для поиска заголовочных файлов
include_directories(include)

# Добавляем исполняемый файл
add_executable(Battleship
    main.cpp
    src/tools.cpp
)

# Линковка с pthread для работы с многопоточностью и синхронизацией
find_package(Threads REQUIRED)
target_link_libraries(Battleship PRIVATE Threads::Threads)

# Добавление флага для использования POSIX API
if(UNIX)
    target_compile_definitions(Battleship PRIVATE -D_POSIX_C_SOURCE=200809L)
endif()

# Установка директории вывода исполняемого файла
set_target_properties(Battleship PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CP
)

# Копирование config.json в директорию сборки
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.json 
    ${CMAKE_BINARY_DIR}/CP/config.json 
    COPYONLY
)
