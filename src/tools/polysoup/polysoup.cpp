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
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "polysoup.h"

enum TokenType
{
	LBRACE,
	RBRACE,
	LPAREN,
	RPAREN,
	NUMBER,
	STRING,
	COMMENT,
	TEXNAME,
	UNKNOWN,
	END_OF_INPUT
};


struct Token
{
	TokenType type;
};

struct Vertex
{
	double x, y, z;
};

struct TextureData
{
	std::string name;
	double xOffset, yOffset;
	double rotation;
	double xScale, yScale;
};

struct Face
{
	Vertex vertices[3];     // Define the plane.
	std::string textureName;
	double xOffset, yOffset;
	double rotation;
	double xScale, yScale;
};

struct Brush
{
	std::vector<Face> faces;
};

struct Property
{
	std::string key;
	std::string value;
};

struct Entity
{
	std::vector<Property> properties;
	std::vector<Brush>    brushes;
};

struct Map
{
	std::vector<Entity> entities;
};

static int                    g_InputLength;

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

int advanceCursor(int* pos, int steps)
{
	int advanceable = g_InputLength - *pos+steps;
	if (advanceable > 0) {
		*pos += advanceable;
		return advanceable;
	}
	return 0;
}

void advanceToNextNonWhitespace(char* c, int* pos)
{
	char* cur = c + *pos;
	while (isspace(*cur)) {
		cur++; *pos += 1;
	}
}

void advanceToNextWhitespaceOrLinebreak(char** c, int* pos)
{
	while (!isspace(**c) && **c != '\n' && **c != '\r') {
		*c += 1; *pos += 1;
	}
}

void skipLinebreak(char* c, int* pos)
{
	char* cur = c + *pos;
	while (*cur == '\r' || *cur == '\n') {
		cur++; *pos += 1;
	}
}

void advanceToNextLine(char* c, int* pos)
{
	char* cur = c + *pos;
	while (*cur != '\r' && *cur != '\n') {
		cur++; *pos += 1;
	}
	if (*cur == '\r') {
		cur++; *pos += 1;
	}
	if (*cur == '\n') {
		cur++; *pos += 1;
	}
}

std::string getString(char*c, int* pos)
{
	char* cur = c + *pos;

	cur++; *pos += 1; // advance over "
	std::string result = "";
	while (*cur != '\"') {
		result += *cur; *pos += 1; cur++;
	}
	*pos += 1; // advance over "

	return result;
}

TokenType getToken(char* c, int* pos)
{	
	if (*pos >= g_InputLength) {
		return END_OF_INPUT;
	}

	TokenType result = UNKNOWN;

	advanceToNextNonWhitespace(c, pos);
	skipLinebreak(c, pos);

	while (*(c + *pos) == '/') { // Skip over all comments
		advanceToNextLine(c, pos);
		advanceToNextNonWhitespace(c, pos);
	}

	char* cur = c + *pos;

	if (*cur == '{') {
		result = LBRACE;
	}
	else if (*cur == '}') {
		result = RBRACE;
	}
	else if (*cur == '(') {
		result = LPAREN;
	}
	else if (*cur == ')') {
		result = RPAREN;
	}
	else if (*cur == '"') {
		result = STRING;
	}
	else if (*cur >= '0' && *cur <= '9' || *cur == '-') { // TODO: MINUS own token and check again if really NUMBER
		result = NUMBER;
	}
	else if (*cur >= 0x21 && *cur <= 0x7E) { // '!' - '~'
		result = TEXNAME;
	}
	else {
		result = UNKNOWN;
		*pos += 1;
	}

	return result;
}

std::string tokenToString(TokenType tokenType)
{
	switch (tokenType)
	{
	case LBRACE: return "LBRACE"; break;
	case RBRACE: return "RBRACE"; break;
	case LPAREN: return "LPAREN"; break;
	case RPAREN: return "RPAREN"; break;
	case NUMBER: return "NUMBER"; break;
	case STRING: return "STRING"; break;
	case COMMENT: return "COMMENT"; break;
	case TEXNAME: return "TEXNAME"; break;
	case UNKNOWN: return "UNKNOWN"; break;
	default: return std::string("!!! H E L P !!!");
	}
}

bool check(TokenType got, TokenType expected)
{
	if (expected != got) {
		fprintf(stderr, "ERROR: Expected Token: %s, but got: %s\n",
			tokenToString(expected).c_str(), tokenToString(got).c_str());
		return false;
	}

	return true;
}



double parseNumber(char* c, int* pos)
{
	char* cur = c + *pos;
	std::string number = "";
	while (*cur == '-' || *cur >= '0' && *cur <= '9' || *cur == '.') {
		number += *cur; cur++; *pos += 1;
	}
	return atof(number.c_str());
}

Vertex getVertex(char* c, int* pos)
{
	Vertex v = { };
	std::vector<double> values;
	for (int i = 0; i < 3; i++) { // A face is defined by 3 vertices.
		advanceToNextNonWhitespace(c, pos);
		double value = parseNumber(c, pos);
		values.push_back(value);
	}

	v.x = values[0];
	v.y = values[1];
	v.z = values[2];

	return v;
}

double getNumber(char* c, int* pos)
{
	char* cur = c + *pos;
	std::string number = "";
	while (*cur == '-' || *cur >= '0' && *cur <= '9' || *cur == '.') {
		number += *cur; cur++; *pos += 1;
	}
	return atof(number.c_str());
}

std::string getTextureName(char* c, int* pos)
{
	char* cur = c + *pos;

	std::string textureName = "";
	char* end = cur;
	advanceToNextWhitespaceOrLinebreak(&end, pos);
	while (cur != end) {
		textureName += *cur; cur++;
	}

	return textureName;
}

Property getProperty(char* c, int* pos) 
{
	check(getToken(c, pos), STRING);
	std::string key = getString(c, pos);
	check(getToken(c, pos), STRING);
	std::string value = getString(c, pos);

	return { key, value };
}

Face getFace(char* c, int* pos) 
{
	Face face = { };

	for (size_t i = 0; i < 3; ++i) {
		check(getToken(c, pos), LPAREN); *pos += 1;
		check(getToken(c, pos), NUMBER);
		double x = getNumber(c, pos);
		check(getToken(c, pos), NUMBER);
		double y = getNumber(c, pos);
		check(getToken(c, pos), NUMBER);
		double z = getNumber(c, pos);
		check(getToken(c, pos), RPAREN); *pos += 1;
		face.vertices[i] = {x, y, z};
	}

	check(getToken(c, pos), TEXNAME);
	face.textureName = getTextureName(c, pos);
	check(getToken(c, pos), NUMBER);
	face.xOffset = getNumber(c, pos);
	check(getToken(c, pos), NUMBER);
	face.yOffset = getNumber(c, pos);
	check(getToken(c, pos), NUMBER);
	face.rotation = getNumber(c, pos);
	check(getToken(c, pos), NUMBER);
	face.xScale = getNumber(c, pos);
	check(getToken(c, pos), NUMBER);
	face.yScale = getNumber(c, pos);

	return face;
}

Brush getBrush(char* c, int* pos)
{
	Brush brush = { };

	while (getToken(c, pos) == LPAREN) {
		brush.faces.push_back(getFace(c, pos));
	}
	return brush;
}

/** 
* I assume that the grammar does not allow a brush *before* a property within an entity!
*/
Entity getEntity(char* c, int* pos)
{
	Entity e = { }; 

	while (getToken(c, pos) == STRING) {
		e.properties.push_back(getProperty(c, pos));
	}

	while (getToken(c, pos) == LBRACE) {
		*pos += 1;
		e.brushes.push_back(getBrush(c, pos));
		check(getToken(c, pos), RBRACE);
		*pos += 1;
	}

	if (getToken(c, pos) == END_OF_INPUT) { // TODO: Needed?
		return { };
	}; 

	check(getToken(c, pos), RBRACE); *pos += 1;

	return e;
}

Map getMap(char* mapData, size_t mapDataLength)
{
	Map map = {};

	int pos = 0;
	while (getToken(&mapData[0], &pos) == LBRACE) {
		pos++;
		map.entities.push_back(getEntity(&mapData[0], &pos));
	}
	
	return map;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		fprintf(stderr, "No .map provided! Usage:\npolysoup <mapfile>");
		exit(-1);
	}
	std::string mapData = loadTextFile(argv[1]);
	size_t inputLength = mapData.length();
	g_InputLength = inputLength;
	size_t inputSizeInBytes = mapData.size();
	Map map = getMap(&mapData[0], inputLength);
	
	printf("done!\n");

	return 0;
}