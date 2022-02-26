#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "polysoup.h"


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
	std::vector<Vertex> vertices;
	TextureData texData;
};

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
	UNKNOWN
};

struct Token
{
	TokenType type;
	union {
		Face face;
	};
};

struct Map
{
	std::vector<Face> faces;
	// TODO: Other entities with properties such as: "classname" -> "info_player_start"
};

static std::vector<TokenType> g_Tokens;

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

void advanceToNextNonWhitespace(char** c, int* pos)
{
	while (isspace(**c)) {
		*c += 1; *pos += 1;
	}
}

void advanceToNextWhitespaceOrLinebreak(char** c, int* pos)
{
	while (!isspace(**c) && **c != '\n' && **c != '\r') {
		*c += 1; *pos += 1;
	}
}

void advanceToNextLine(char** c, int* pos)
{
	while (**c != '\r' && **c != '\n') {
		*c += 1; *pos += 1;
	}
	if (**c == '\r') {
		*c += 1; *pos += 1;
	}
	if (**c == '\n') {
		*c += 1; *pos += 1;
	}
}

TokenType getToken(char * c, int * pos)
{
	TokenType result = UNKNOWN;


	if (*c == '{') {
		result = LBRACE;
		g_Tokens.push_back(result);
	}
	else if (*c == '}') {
		result = RBRACE;
	}
	else if (*c == '(') {
		result = LPAREN;
		g_Tokens.push_back(result);
	}
	else if (*c == ')') {
		result = RPAREN;
	}
	else if (*c == '"') {
		result = STRING;
	}
	else if (*c >= '0' && *c <= '9') {
		result = NUMBER;
	}
	else if (*c >= 'A' && *c <= 'z') {
		result = TEXNAME;
	}
	else {
		result = UNKNOWN;
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
	default: return std::string("");
	}
}

bool matchToken(char* c, int* pos, TokenType expected)
{
	advanceToNextNonWhitespace(&c, pos);
	while (*c == '/') { // Skip over all comments
		advanceToNextLine(&c, pos);
		advanceToNextNonWhitespace(&c, pos);
	}

	TokenType got = getToken(c, pos);
	c++; *pos += 1;
	if (expected != got) {
		fprintf(stderr, "ERROR: Expected Token: %s, but got: %s\n", 
			tokenToString(expected).c_str(), tokenToString(got).c_str());
		return false;
	}

	return true;
}

std::string getString(char** c, int* pos)
{
	*c += 1; *pos += 1; // advance over "
	std::string result = "";
	while (**c != '\"') {
		result += **c; *pos += 1; *c += 1;
	}
	*c += 1; *pos += 1; // advance over "

	return result;
}

double parseNumber(char** c, int* pos)
{
	std::string number = "";
	while (**c == '-' || **c >= '0' && **c <= '9' || **c == '.') {
		number += **c; *c += 1; *pos += 1;
	}
	return atof(number.c_str());
}

Vertex getVertex(char* c, int* pos)
{
	Vertex v = { };
	std::vector<double> values;
	for (int i = 0; i < 3; i++) { // A face is defined by 3 vertices.
		advanceToNextNonWhitespace(&c, pos);
		double value = parseNumber(&c, pos);
		values.push_back(value);
	}

	v.x = values[0];
	v.y = values[1];
	v.z = values[2];

	return v;
}

double getNumber(char* c, int* pos)
{
	return parseNumber(&c, pos);
}

std::string getTextureName(char* c, int* pos)
{
	std::string textureName = "";
	char* end = c;
	advanceToNextWhitespaceOrLinebreak(&end, pos);
	while (c != end) {
		textureName += *c; c++;
	}

	return textureName;
}

void getProperty(char* c, int* pos) {
	matchToken(c, pos, STRING);
	matchToken(c, pos, STRING);
}

void getBrush(char* c, int* pos) {
	Face face = {};
	TokenType token = UNKNOWN;

	Vertex v0 = getVertex(c, pos);
	matchToken(c, pos, RPAREN);

	matchToken(c, pos, LPAREN);
	Vertex v1 = getVertex(c, pos);
	matchToken(c, pos, RPAREN);

	matchToken(c, pos, LPAREN);
	Vertex v2 = getVertex(c, pos);
	matchToken(c, pos, RPAREN);

	TextureData tex = {};
	token = getToken(c, pos);
	tex.name = getTextureName(c, pos);
	token = getToken(c, pos); // Number

	tex.xOffset = getNumber(c, pos);
	token = getToken(c, pos); // Number
	tex.yOffset = getNumber(c, pos);
	token = getToken(c, pos); // Number
	tex.rotation = getNumber(c, pos);
	token = getToken(c, pos); // Number
	tex.xScale = getNumber(c, pos);
	token = getToken(c, pos); // Number
	tex.yScale = getNumber(c, pos);

	face.vertices = std::vector<Vertex>{ v0, v1, v2 };
	face.texData = tex;
}

void getEntity(char* c, int* pos)
{
	advanceToNextNonWhitespace(&c, pos);

	while (matchToken(c, pos, STRING)) {
		getProperty(c, pos);
	}
	while (matchToken(c, pos, LPAREN)) {
		getBrush(c, pos);
	}
}

Map getMap(char* mapData, size_t mapDataLength)
{
	Map map = {};
	std::vector<Face> faces;

	int pos = 0;

	while (matchToken(&mapData[pos], &pos, LBRACE)) {		
		getEntity(&mapData[pos], &pos);
		matchToken(&mapData[pos], &pos, RBRACE);
	}

	while (pos < mapDataLength) {

		TokenType token = getToken(&mapData[pos], &pos);
		if (token == LBRACE) { // Entity or Brush
			//g_Tokens.push_back(token);
		}
		else if (token == RBRACE) {
			if (g_Tokens.back() == LBRACE) {
				g_Tokens.pop_back();
			}
			else {
				fprintf(stderr, "WARNING: Expected Token: }, but got: %s\n", tokenToString(token).c_str());
			}
		}
		else if (token == STRING) { // Property
			//std::string key = getString(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos);
			//std::string value = getString(&mapData[pos], &pos);
		}
		else if (token == LPAREN) { // Face
			Face face = {};

			Vertex v0 = getVertex(&mapData[pos], &pos);
			matchToken(&mapData[pos], &pos, RPAREN);

			matchToken(&mapData[pos], &pos, LPAREN);
			Vertex v1 = getVertex(&mapData[pos], &pos);
			matchToken(&mapData[pos], &pos, RPAREN);

			matchToken(&mapData[pos], &pos, LPAREN);
			Vertex v2 = getVertex(&mapData[pos], &pos);
			matchToken(&mapData[pos], &pos, RPAREN);
			
			TextureData tex = {};
			token = getToken(&mapData[pos], &pos);
			tex.name = getTextureName(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos); // Number

			tex.xOffset = getNumber(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos); // Number
			tex.yOffset = getNumber(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos); // Number
			tex.rotation = getNumber(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos); // Number
			tex.xScale = getNumber(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos); // Number
			tex.yScale = getNumber(&mapData[pos], &pos);

			face.vertices = std::vector<Vertex>{ v0, v1, v2 };
			face.texData = tex;

			faces.push_back(face);
		}
		
		pos++;
	}
	
	map.faces = faces;
	
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
	size_t inputSizeInBytes = mapData.size();
	Map map = getMap(&mapData[0], inputLength);
	

	printf("done!\n");
	//getchar();

	return 0;
}