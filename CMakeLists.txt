cmake_minimum_required(VERSION 3.5.1)
project(cv_challenge)

include(cmake/Modules/build_external_project.cmake)
build_external_project(OpenCV "${CMAKE_CURRENT_BINARY_DIR}" git@github.com:Itseez/opencv.git -DCMAKE_BUILD_TYPE=RELEASE -DWITH_TBB=ON)
find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

set(IMAGES_PATH   "${PROJECT_SOURCE_DIR}")
configure_file(config.h.in ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/config.h)

add_definitions(-Wno-deprecated)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(stdc++fs SHARED IMPORTED)
set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/5/libstdc++fs.a")

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} stdc++fs)