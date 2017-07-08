cmake_minimum_required(VERSION 2.8)

project(d2crawler)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(CURL REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Sql Network)

include_directories(${CURL_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} "main.cpp" "match_crawler.cpp" "update_manager.cpp" "match_data_writer")
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${CURL_LIBRARIES})

qt5_use_modules(${PROJECT_NAME} Core Sql Network)
add_definitions(-std=c++17)
