cmake_minimum_required(VERSION 3.5)

project(chess LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/config.h
	${PROJECT_SOURCE_DIR}/main_window.h
	${PROJECT_SOURCE_DIR}/main_window.cpp
	${PROJECT_SOURCE_DIR}/piece_selector_widget.h
	${PROJECT_SOURCE_DIR}/piece_selector_widget.cpp
	${PROJECT_SOURCE_DIR}/cell.h
	${PROJECT_SOURCE_DIR}/cell.cpp
	${PROJECT_SOURCE_DIR}/piece.h
	${PROJECT_SOURCE_DIR}/piece.cpp
	${PROJECT_SOURCE_DIR}/resource/resource.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
