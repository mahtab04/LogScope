cmake_minimum_required(VERSION 3.23)
project(LogScope VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)

# Use Fedora's system Qt 6; Conan supplies only third-party C++ packages.
find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
find_package(fmt REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)

add_library(logscope_core
    src/LogFileReader.cpp
    src/LogParser.cpp
    src/LogModel.cpp
    src/LogFilterProxyModel.cpp
)
target_include_directories(logscope_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(logscope_core PUBLIC
    Qt6::Widgets
    fmt::fmt
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

add_executable(logscope src/main.cpp src/MainWindow.cpp)
target_link_libraries(logscope PRIVATE logscope_core)

# A tiny CLI program to prove the Conan libraries link correctly.
add_executable(logscope_samplegen tools/sample_generator.cpp)
target_link_libraries(logscope_samplegen PRIVATE
    fmt::fmt spdlog::spdlog nlohmann_json::nlohmann_json
)

include(CTest)
if(BUILD_TESTING)
    add_executable(logscope_tests tests/test_LogParser.cpp)
    target_link_libraries(logscope_tests PRIVATE logscope_core Qt6::Test)
    add_test(NAME parser_smoke COMMAND logscope_tests)
endif()
