cmake_minimum_required(VERSION 3.24)

project(QtDino VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

qt_add_executable(QtDino
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/GameWidget.cpp
    src/GameWidget.h
    src/ResourceManager.cpp
    src/ResourceManager.h
)

target_link_libraries(QtDino PRIVATE Qt6::Widgets Qt6::Multimedia)

add_custom_command(TARGET QtDino POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:QtDino>/Resources
)

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET QtDino POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE}
            --no-translations
            --no-compiler-runtime
            $<TARGET_FILE:QtDino>
    )
endif()
