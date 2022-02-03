cmake_minimum_required(VERSION 3.10)
project(Game VERSION 1.0)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE
     ${CMAKE_CURRENT_SOURCE_DIR} *.c??)
file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE
     ${CMAKE_CURRENT_SOURCE_DIR} *.h)    

add_library(Game SHARED
	${SRC_FILES}
	${HEADER_FILES}
	exports.def)
	
target_include_directories(Game
	PUBLIC ../Engine
	PUBLIC ../dependencies
)
