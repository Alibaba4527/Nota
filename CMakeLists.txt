# Nota: one executable, system Qt 6 Widgets, C++20.
cmake_minimum_required(VERSION 3.21)

project(Nota VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Test)

add_executable(nota
    src/main.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/notation/NotationView.h
    src/notation/NotationView.cpp
    src/notation/Layout.h
    src/notation/Layout.cpp
    src/notation/Glyphs.h
    src/notation/Glyphs.cpp
    src/model/Document.h
    src/model/Document.cpp
    src/model/Commands.h
    src/model/Commands.cpp
)

target_include_directories(nota PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(nota PRIVATE Qt6::Widgets)

add_executable(tst_model tests/tst_model.cpp)
target_include_directories(tst_model PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tst_model PRIVATE Qt6::Test)

if(MSVC)
    target_compile_options(nota PRIVATE /W4)
    target_compile_options(tst_model PRIVATE /W4)
else()
    target_compile_options(nota PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(tst_model PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME tst_model COMMAND tst_model)
