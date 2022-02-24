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
	UNKNOWN
};

struct Token
{
	TokenType type;
	union {
		Face face;
	};
};

static std::string loadTextFile(std::string file)
{
	std::ifstream iFileStream;
	std::stringstream ss;
	iFileStream.open(file, std::ifstream::in);
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

TokenType getToken(char * c, int * pos)
{
	advanceToNextNonWhitespace(&c, pos);

	if (*c == '{') {
		*pos += 1;
		return LBRACE;
	}
	else if (*c == '}') {
		*pos += 1;
		return RBRACE;
	}
	else if (*c == '(') {
		*pos += 1;
		return LPAREN;
	}
	else if (*c == ')') {
		*pos += 1;
		return RPAREN;
	}
	else if (*c == '"') {
		*pos += 1;
		return STRING;
	}
	else if (*c >= '0' && *c <= '9') {
		return NUMBER;
	}
	else if (*c == '/') {
		while (*c != '\r' && *c != '\n') {
			c++; *pos += 1;
		}
		return COMMENT;
	}
	else if (*c >= 'A' && *c <= 'z')

	return UNKNOWN;
}

std::string getString(char* c, int* pos)
{
	std::string result = "";
	while (*c != '\"') {
		result += *c; *pos += 1; c++;
	}
	*pos += 1; // one past closing "-character

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

void parse(char* c, int* pos)
{
	*pos += 1;
	if (*c == '\"') { // Property
		std::string key   = getString(c, pos);
		std::string value = getString(c, pos);
	}
	else if (*c == '(') { // Brush
		Vertex v0 = getVertex(c, pos);
		Vertex v1 = getVertex(c, pos);
		Vertex v2 = getVertex(c, pos);
	}
}

std::vector<TokenType> g_Tokens;

int main(int argc, char** argv)
{
	std::string mapData = loadTextFile(argv[1]);
	size_t inputLength = mapData.length();
	size_t inputSizeInBytes = mapData.size();

	std::vector<Face> faces;

	int pos = 0;
	while (pos < inputLength) {

		TokenType token = getToken(&mapData[pos], &pos);	
		if (token == LBRACE) { // Entity or Brush
			g_Tokens.push_back(token);
		}
		else if (token == STRING) { // Property
			std::string key   = getString(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos);
			std::string value = getString(&mapData[pos], &pos);
		}
		else if (token == LPAREN) { // Face
			Face face = {};

			Vertex v0 = getVertex(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos);

			token = getToken(&mapData[pos], &pos);
			Vertex v1 = getVertex(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos);

			token = getToken(&mapData[pos], &pos);
			Vertex v2 = getVertex(&mapData[pos], &pos);
			token = getToken(&mapData[pos], &pos);

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

	printf("done!\n");
	getchar();

	return 0;
}