cmake_minimum_required(VERSION 3.23)
project(LogScope VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)

# Use Fedora's system Qt 6; Conan supplies only third-party C++ packages.
find_package(Qt6 REQUIRED COMPONENTS Widgets)
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
qt_add_resources(logscope "themes"
    PREFIX "/"
    FILES resources/themes/dark.qss resources/themes/light.qss
)

# A tiny CLI program to prove the Conan libraries link correctly.
add_executable(logscope_samplegen tools/sample_generator.cpp)
target_link_libraries(logscope_samplegen PRIVATE
    fmt::fmt spdlog::spdlog nlohmann_json::nlohmann_json
)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest REQUIRED CONFIG)
    include(GoogleTest)
    add_executable(logscope_tests tests/test_LogParser.cpp tests/test_LogModel.cpp tests/test_LogFilterProxyModel.cpp)
    target_link_libraries(logscope_tests PRIVATE logscope_core GTest::gtest_main)
    set_target_properties(logscope_tests PROPERTIES AUTOMOC OFF)
    gtest_discover_tests(logscope_tests)

    add_executable(logscope_record_tests tests/test_LogRecord.cpp)
    target_include_directories(logscope_record_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(logscope_record_tests PRIVATE Qt6::Core GTest::gtest_main)
    set_target_properties(logscope_record_tests PROPERTIES AUTOMOC OFF)
    gtest_discover_tests(logscope_record_tests)

    add_executable(logscope_reader_tests tests/test_LogFileReader.cpp)
    target_link_libraries(logscope_reader_tests PRIVATE logscope_core GTest::gtest_main)
    set_target_properties(logscope_reader_tests PROPERTIES AUTOMOC OFF)
    gtest_discover_tests(logscope_reader_tests)

    # Run tests on each default build, including builds with no source changes.
    add_custom_target(check ALL
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS logscope_tests logscope_record_tests logscope_reader_tests
        USES_TERMINAL
        VERBATIM
    )
endif()
