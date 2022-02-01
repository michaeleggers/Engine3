cmake_minimum_required(VERSION 3.10)
project(Game VERSION 1.0)

add_library(Game SHARED
	mygame.cpp
	exports.def)
	
target_include_directories(Game
	PUBLIC ../Engine
	PUBLIC ../dependencies
)
