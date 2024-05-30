cmake_minimum_required(VERSION 3.29.0)

project(ngate)

set(SRC "src/main.c")

add_executable(${PROJECT_NAME} ${SRC})