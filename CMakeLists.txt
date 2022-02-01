cmake_minimum_required(VERSION 3.10)


project(Engine3dev VERSION 1.0)

find_package(Vulkan REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

if(${WINDOWING} STREQUAL "VKAL_SDL")
	message("Using SDL2...")
	add_subdirectory(src/dependencies/SDL)
	# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/external/SDL)
endif()

add_subdirectory(src/dependencies/vkal)
add_subdirectory(src/Engine)
add_subdirectory(src/Game)

add_dependencies(Engine3 vkal)
add_dependencies(Engine3 Game)