cmake_minimum_required(VERSION 3.10)
project(Engine3 VERSION 1.0)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE
     ${CMAKE_CURRENT_SOURCE_DIR} *.c??)
file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE
     ${CMAKE_CURRENT_SOURCE_DIR} *.h)    

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/vkal)
add_executable(Engine3
	${SRC_FILES}
	${HEADER_FILES}
)
	
target_include_directories(Engine3
    PUBLIC ../dependencies
)

target_link_libraries(Engine3
	PUBLIC vkal
	PUBLIC SDL2
	PUBLIC SDL2main)