cmake_minimum_required(VERSION 3.29.0)

project(ngate)

set(SRC 
${CMAKE_SOURCE_DIR}/src/main.c
${CMAKE_SOURCE_DIR}/src/networking.c
${CMAKE_SOURCE_DIR}/src/config.c)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
PRIVATE ${CMAKE_SOURCE_DIR}/include)