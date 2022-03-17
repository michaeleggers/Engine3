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
#include <stack>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "polysoup.h"

#define MAP_PARSER_IMPLEMENTATION
#include "parser.h"

#define PS_FLOAT_EPSILON	(0.001)



struct Polygon
{
	std::vector<glm::vec3> vertices;
	glm::vec3			   normal;
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

static void writePolysOBJ(std::string fileName, std::vector<Polygon> polys)
{
	std::stringstream faces;
	std::ofstream oFileStream;
	oFileStream.open(fileName, std::ios::out);
	
	oFileStream << "o Quake-map" << std::endl;

	size_t count = 1;
	for (auto p = polys.begin(); p != polys.end(); p++) {
		faces << "f";
		for (auto v = p->vertices.begin(); v != p->vertices.end(); v++) {
			oFileStream << "v " << std::to_string(v->x) << " " << std::to_string(v->y) << " " << std::to_string(v->z) << std::endl;
			faces << " " << count; count++;
		}
		faces << std::endl;
	}

	oFileStream << faces.rdbuf();
	oFileStream.close();
	
}

Plane createPlane(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
	glm::vec3 v0 = p2 - p0;
	glm::vec3 v1 = p1 - p0;
	glm::vec3 n = glm::normalize(glm::cross(v0, v1));
	float d = -glm::dot(n, p0);

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
	glm::vec3 n0xn1 = glm::cross(p0.n, p1.n);
	float det = glm::dot(n0xn1, p2.n);

	if (fabs(det) < PS_FLOAT_EPSILON) // Early out if planes do not intersect at single point
		return false;

	*intersectionPoint = (
		-p0.d * (glm::cross(p1.n, p2.n))
		-p1.d * (glm::cross(p2.n, p0.n))
		-p2.d * (glm::cross(p0.n, p1.n))
		) / det;

	return true;
}

bool vec3IsEqual(const glm::vec3& lhs, const glm::vec3& rhs) {
	return ( glm::abs(lhs.x - rhs.x) < PS_FLOAT_EPSILON 
		&& glm::abs(lhs.y - rhs.y) < PS_FLOAT_EPSILON
		&& glm::abs(lhs.z - rhs.z) < PS_FLOAT_EPSILON );
}

void insertVertexToPolygon(glm::vec3 v, Polygon* p)
{
	auto v0 = p->vertices.begin();
	if (v0 == p->vertices.end()) {
		p->vertices.push_back(v);
		return;
	}

	for (; v0 != p->vertices.end(); v0++) {
		if (vec3IsEqual(v, *v0)) { // TODO: Is this check actually needed??
			return;
		}
	}
	
	p->vertices.push_back(v);
}

bool isPointValid(Brush brush, glm::vec3 intersectionPoint)
{
	for (int i = 0; i < brush.faces.size(); i++) {
		Face face = brush.faces[i];
		Plane plane = convertFaceToPlane(face);
		glm::vec3 a = glm::normalize(intersectionPoint - plane.p0);
		float dotProd = glm::dot(plane.n, a);
		if (glm::dot(plane.n, intersectionPoint) + plane.d > PS_FLOAT_EPSILON) // FIXME: HOW DO WE GET IT NUMERICALLY GOOD ENOUGH??
			return false;
	}

	return true;
}

std::vector<Polygon> createPolysoup(Map map)
{
	std::vector<Polygon> polys;
	for (auto e = map.entities.begin(); e != map.entities.end(); e++) {
		for (auto b = e->brushes.begin(); b != e->brushes.end(); b++) {
			int faceCount = b->faces.size();
			for (int i = 0; i <  faceCount; i++) {
				Plane p0 = convertFaceToPlane(b->faces[i]);
				Polygon poly = {};
				poly.normal = p0.n;
				for (int j = 0; j < faceCount; j++) {
					Plane p1 = convertFaceToPlane(b->faces[j]);
					for (int k = 0; k < faceCount; k++) {
						glm::vec3 intersectionPoint;
						Plane p2 = convertFaceToPlane(b->faces[k]);
						if (i != k && k != j && i != j) {
							if (intersectThreePlanes(p0, p1, p2, &intersectionPoint)) {
								if (isPointValid(*b, intersectionPoint)) {
									//poly.vertices.push_back(intersectionPoint);
									insertVertexToPolygon(intersectionPoint, &poly);
								}
							}
						}
					}
				}
				if (poly.vertices.size() > 0)
					polys.push_back(poly);
			}
		}
	}

	return polys;
}

bool isAngleLegal(glm::vec3 center, glm::vec3 v0, glm::vec3 v1)
{
	Plane polyPlane = createPlane(center, v0, v1);
	glm::vec3 b = glm::normalize(v1 - center);
	Plane p = createPlane(center, v0, center + polyPlane.n);
	if (glm::dot(p.n, b) < -0.0001) {
		return false;
	}

	return true;
}

float getAngle(glm::vec3 center, glm::vec3 v0, glm::vec3 v1)
{
	glm::vec3 a = glm::normalize(v0 - center);
	glm::vec3 b = glm::normalize(v1 - center);

	return glm::acos(glm::dot(a, b));
}

Polygon sortVerticesCCW(Polygon poly)
{
	Polygon result;
	size_t vertCount = poly.vertices.size();
	
	if (vertCount < 3)
		return poly; // Actually not a valid polygon

	// Center of poly
	glm::vec3 center(0.0f);
	for (auto v = poly.vertices.begin(); v != poly.vertices.end(); v++) {
		center += *v;
	}
	center /= vertCount;

	size_t closestVertexID = 0;
	for (size_t i = 0; i < vertCount-3; i++) {
		glm::vec3 v0 = poly.vertices[i]; // Find next vertex to v0 with smallest angle
		size_t smallesAngleIndex = 0;
		float smallestAngle = glm::two_pi<float>();
		for (size_t j = i+1; j < vertCount-1; j++) {
			if (isAngleLegal(center, v0, poly.vertices[j])) {
				float angle = getAngle(center, v0, poly.vertices[j]);
				if (angle < smallestAngle) {
					smallestAngle = angle;
					smallesAngleIndex = j;
				}
			}
		}
		std::swap(poly.vertices[i+1], poly.vertices[smallesAngleIndex]); // Vertex number j+1 has a smaller angle than the one coming befor it -> swap
	}

	// Fix winding
	glm::vec3 a = poly.vertices[1] - poly.vertices[0];
	glm::vec3 b = poly.vertices[2] - poly.vertices[0];
	glm::vec3 normal = glm::normalize(glm::cross(a, b));
	if (glm::dot(normal, poly.normal) < PS_FLOAT_EPSILON) {
		std::reverse(poly.vertices.begin(), poly.vertices.end());
	}

	return poly;
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
* v0 is the 'provoking vertex'. It is the anchor-point of the triangle fan.
* Note that a lot of redundant data is generated: (v0, v1, v2), (v0, v2, v3), (v0, v3, v4).
* 
* TODO: Fix this with indexed data.
*/
std::vector<Polygon> triangulate(std::vector<Polygon> polys)
{
	std::vector<Polygon> tris = { };

	for (auto p = polys.begin(); p != polys.end(); p++) {
		Polygon sortedPoly = sortVerticesCCW(*p);
		size_t vertCount = sortedPoly.vertices.size();		
		glm::vec3 provokingVert = sortedPoly.vertices[0];
		for (size_t i = 2; i < vertCount; i++) {
			Polygon poly = { };
			poly.vertices.push_back(provokingVert);
			poly.vertices.push_back(sortedPoly.vertices[i - 1]);
			poly.vertices.push_back(sortedPoly.vertices[i]);
			tris.push_back(poly);
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
	writePolysOBJ("tris.obj", tris);

	printf("done!\n");

	return 0;
}