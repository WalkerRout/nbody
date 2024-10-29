cmake_minimum_required(VERSION 3.15)
project(nbody C)

find_package(raylib REQUIRED)

file(GLOB SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES})

find_library(LIBM m)
if (LIBM)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBM})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)

target_include_directories(${PROJECT_NAME} PUBLIC include)