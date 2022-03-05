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
#include <ostream>
#include <sstream>
#include <vector>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "polysoup.h"

#define MAP_PARSER_IMPLEMENTATION
#include "parser.h"

#define PS_FLOAT_EPSILON	(0.0001f)



struct Polygon
{
	std::vector<glm::vec3> vertices;
};

struct Plane
{
	glm::vec3 n;
	glm::vec3 p0;
	float d;		// = n dot p0. Just for convenience.
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

static void writePolys(std::string fileName, std::vector<Polygon> polys)
{
	std::ofstream oFileStream;
	oFileStream.open(fileName, std::ios::binary | std::ios::out);

	uint32_t numPolys = polys.size();
	oFileStream.write((char*)&numPolys, sizeof(uint32_t));

	for (auto p = polys.begin(); p != polys.end(); p++) {
		for (auto v = p->vertices.begin(); v != p->vertices.end(); v++) {
			oFileStream.write((char*) & *v, sizeof(glm::vec3));
		}
	}

	oFileStream.close();
}

Plane createPlane(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
	glm::vec3 v0 = p2 - p0;
	glm::vec3 v1 = p1 - p0;
	glm::vec3 n = glm::normalize(glm::cross(v0, v1));
	float d = glm::dot(n, p0);

	return { n, p0, d };
}

static inline glm::vec3 convertVertexToVec3(Vertex v)
{
	return glm::vec3(v.x, v.y, v.z);
}

Plane convertFaceToPlane(Face face)
{
	glm::vec3 p0 = convertVertexToVec3(face.vertices[0]);
	glm::vec3 p1 = convertVertexToVec3(face.vertices[1]);
	glm::vec3 p2 = convertVertexToVec3(face.vertices[2]);
	return createPlane(p0, p1, p2);
}

bool intersectThreePlanes(Plane p0, Plane p1, Plane p2, glm::vec3* intersectionPoint)
{	
	glm::vec3 n1xn2 = glm::cross(p0.n, p1.n);
	float det = glm::dot(n1xn2, p2.n);

	if (fabs(det) < PS_FLOAT_EPSILON) // Early out if planes do not intersect at single point
		return false;

	*intersectionPoint = (
		p0.d * (glm::cross(p2.n, p1.n))
		+ p1.d * (glm::cross(p0.n, p2.n))
		+ p2.d * (glm::cross(p1.n, p0.n))
		) / det;

	return true;
}

bool vec3IsEqual(const glm::vec3& lhs, const glm::vec3& rhs) {
	return ( (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) );
}

void insertVertexToPolygon(glm::vec3 v, Polygon* p)
{
	auto v0 = p->vertices.begin();
	if (v0 == p->vertices.end()) {
		p->vertices.push_back(v);
		return;
	}

	for (; v0 != p->vertices.end(); v0++) {
		if (vec3IsEqual(v, *v0)) {
			return;
		}
	}
	
	p->vertices.push_back(v);
}

std::vector<Polygon> createPolysoup(Map map)
{
	std::vector<Polygon> polys;
	for (auto e = map.entities.begin(); e != map.entities.end(); e++) {
		for (auto b = e->brushes.begin(); b != e->brushes.end(); b++) {
			for (auto f0 = b->faces.begin(); f0 != b->faces.end(); f0++) {
				Polygon poly = {};
				for (auto f1 = b->faces.begin(); f1 != b->faces.end(); f1++) {
					for (auto f2 = b->faces.begin(); f2 != b->faces.end(); f2++) {
						glm::vec3 intersectionPoint;
						Plane p0 = convertFaceToPlane(*f0);
						Plane p1 = convertFaceToPlane(*f1);
						Plane p2 = convertFaceToPlane(*f2);
						if (intersectThreePlanes(p0, p1, p2, &intersectionPoint)) {
							//poly.vertices.push_back(intersectionPoint);
							insertVertexToPolygon(intersectionPoint, &poly);
						}
					}
				}
				polys.push_back(poly);
			}
		}
	}

	return polys;
}

/*
* Assumes only convex polygons -> use trivial triangulation approach where
* a triangle-fan gets built, eg:
* 
*       v2______v3                        v2______v3
*       /       |                         /|     /|
*      /        |                        / |    / |
*   v1/         |       -->           v1/  |   /  |
*     \         |                       \  |  /   |
*      \        |                        \ | /    | 
*       \_______|                         \|/_____|
*        v0     v4                         v0     v4     
* 
* v0 is the 'provoking vertex'. It is the ankor-point of the triangle fan.
* Note that a lot of redundant data is generated: (v0, v1, v2), (v0, v2, v3), (v0, v3, v4).
* 
* TODO: Fix this with indexed data.
*/
std::vector<Polygon> triangulate(std::vector<Polygon> polys)
{
	std::vector<Polygon> tris = { };
	for (auto p = polys.begin(); p != polys.end(); p++) {
		
		size_t vertCount = p->vertices.size();
		if (vertCount > 3) {
			glm::vec3 provokingVert = p->vertices[0];
			for (size_t i = 2; i < vertCount; i++) {
				Polygon poly = { };
				poly.vertices.push_back(provokingVert);
				poly.vertices.push_back(p->vertices[i - 1]);
				poly.vertices.push_back(p->vertices[i]);
				tris.push_back(poly);
			}
		}
		else {
			tris.push_back(*p);	
		}		
	}

	return tris;
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
	std::vector<Polygon> polysoup = createPolysoup(map);
	std::vector<Polygon> tris = triangulate(polysoup);
	writePolys("tris.bin", tris);

	printf("done!\n");

	return 0;
}