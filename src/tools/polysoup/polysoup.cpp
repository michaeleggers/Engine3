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

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "polysoup.h"

#define MAP_PARSER_IMPLEMENTATION
#include "parser.h"

struct Polygon
{
	std::vector<glm::vec3> v;
};

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

Polygon createPolygon(Vertex f[], Vertex f2[], Polygon& poly)
{
	glm::vec3 v0 = glm::vec3(f[0].x, f[0].y, f[0].z);
	glm::vec3 v1 = glm::vec3(f[1].x, f[1].y, f[1].z);
	glm::vec3 v2 = glm::vec3(f[2].x, f[2].y, f[2].z);

	return {};
}

std::vector<Polygon> createPolysoup(Map map)
{
	std::vector<Polygon> polys;
	for (auto e = map.entities.begin(); e != map.entities.end(); e++) {
		for (auto b = e->brushes.begin(); b != e->brushes.end(); b++) {
			for (auto f = b->faces.begin(); f != b->faces.end(); f++) {
				Polygon poly = {};
				for (auto f2 = b->faces.begin(); f2 != b->faces.end(); f2++) {
					createPolygon(f->vertices, f2->vertices, poly);
				}
				polys.push_back(poly);
			}
		}
	}

	return polys;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		fprintf(stderr, "No .map provided! Usage:\npolysoup <mapfile>");
		exit(-1);
	}

	std::string mapData = loadTextFile(argv[1]);
	size_t inputLength = mapData.length();
	Map map = getMap(&mapData[0], inputLength);
	auto polysoup = createPolysoup(map);

	printf("done!\n");

	return 0;
}