cmake_minimum_required(VERSION 3.10)
project(Engine3 VERSION 1.0)

ADD_SRC_HEADER_FILES()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/vkal
	# ${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/SDL/include
)

add_executable(Engine3
	${SRC_FILES}
	${HEADER_FILES}
)
	
target_include_directories(Engine3
    PUBLIC ../dependencies
)

target_link_libraries(Engine3
	PUBLIC vkal
	# PUBLIC SDL2
	# PUBLIC SDL2main
)