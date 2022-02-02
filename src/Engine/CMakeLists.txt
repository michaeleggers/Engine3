cmake_minimum_required(VERSION 3.10)
project(Engine3 VERSION 1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/vkal)
add_executable(Engine3
	cengine_service.cpp
	cengine_service.h
	interface.h
	platform.cpp
	platform.h
	main.cpp)
	
target_include_directories(Engine3
    PUBLIC ../dependencies
)

target_link_libraries(Engine3
	PUBLIC vkal
	PUBLIC SDL2
	PUBLIC SDL2main)