cmake_minimum_required(VERSION 3.5)

project(BrickGameApp)

set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

include_directories(
    ${PROJECT_ROOT}
    ${PROJECT_ROOT}/brick_game
    ${PROJECT_ROOT}/brick_game/tetris
    ${PROJECT_ROOT}/brick_game/snake
)

link_directories(${PROJECT_ROOT})

add_definitions(-DQT_NO_NCURSES_MACROS)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(SQLite3 REQUIRED)

set(SOURCES
    main.cc
    mainwindow.cc
)

set(HEADERS
    mainwindow.h
)

add_executable(desktop_tetris ${SOURCES} ${HEADERS})
target_link_libraries(desktop_tetris PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    brickgame_tetris
    SQLite::SQLite3
)

add_executable(desktop_snake ${SOURCES} ${HEADERS})
target_link_libraries(desktop_snake PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    brickgame_snake
    SQLite::SQLite3
)