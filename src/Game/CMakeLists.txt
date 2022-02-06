cmake_minimum_required(VERSION 3.10)
project(Game VERSION 1.0)

ADD_SRC_HEADER_FILES()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/vkal
	${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/SDL/include
)

add_library(Game SHARED
	${SRC_FILES}
	${HEADER_FILES}
	exports.def)
	
target_include_directories(Game
	PUBLIC ../Engine
	PUBLIC ../dependencies
)

target_link_libraries(Game
	PUBLIC vkal
)
