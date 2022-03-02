/*
* Quake MAP parser.
* 
* Grammar:
* ----------------------------------------------------------------
* Terminals = {key, value, plane, texture, offset, scale, rotation}
* Non Terminals = {map, entity, property, brush, face}

* map         -> entity *entity
* entity      -> *property *brush
* property    -> key value
* brush       -> *face
* face        -> plane texture-name xOffset yOffset rotation xScale yScale
* 
* A plane is defined by 3 vertices.
* A property is always a pair of strings.
* 
* For more information, see: https://quakewiki.org/wiki/Quake_Map_Format
*/


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "polysoup.h"

#define MAP_PARSER_IMPLEMENTATION
#include "parser.h"

static std::string loadTextFile(std::string file)
{
	std::ifstream iFileStream;
	std::stringstream ss;
	iFileStream.open(file, std::ifstream::in);
	if (iFileStream.fail()) {
		fprintf(stderr, "Unable to open file: %s!\nExiting...", file.c_str());
		exit(-1);
	}
	ss << iFileStream.rdbuf();
	std::string data = ss.str();
	iFileStream.close();

	return data;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		fprintf(stderr, "No .map provided! Usage:\npolysoup <mapfile>");
		exit(-1);
	}
	int pos = 0;
	int steps = 10;

	std::string mapData = loadTextFile(argv[1]);
	size_t inputLength = mapData.length();
	size_t inputSizeInBytes = mapData.size();
	Map map = getMap(&mapData[0], inputLength);
	
	printf("done!\n");

	return 0;
}