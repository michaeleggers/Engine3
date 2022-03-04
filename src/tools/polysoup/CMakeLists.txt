cmake_minimum_required(VERSION 3.10)
project(Polysoup VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")


# FUCK CMAKE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# ADD_SRC_HEADER_FILES()
# file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE
#     ${CMAKE_CURRENT_SOURCE_DIR} *.c??)
# file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE
#     ${CMAKE_CURRENT_SOURCE_DIR} *.h)    

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../dependencies/
)

add_executable(Polysoup
    polysoup.cpp
    parser.h
)


# target_link_libraries(Polysoup
# 	PUBLIC vkal
# 	PUBLIC SDL2
# 	PUBLIC SDL2main)