import sys
from enum import Enum

class TokenType(Enum):
    COMMENT = 0
    ENTITY = 1
    VERTEX = 2
    NUMBER = 3
    STRING = 4
    BRUSH = 5
    NONE = 6

class Token:
    def __init__(self, tokenType: TokenType=TokenType.NONE):
        self.type = tokenType
        self.string = None
        self.number = None
        self.brush = None
        self.vertex = None

def match(expected, got):
    return expected == got

def getNextToken(line: str):
    if line.startswith("//"):
        return Token(TokenType.COMMENT)
    if line.startswith("{"):
        return Token(TokenType.ENTITY)
    if line.startswith("\""):
        return Token(TokenType.STRING)
    if line.startswith("("):
        return Token(TokenType.VERTEX)
    
    return Token(TokenType.NONE)

def getVertex(line):
    v = []
    start = line.find("(")
    end = line.find(")")
    line = line[start+1:end].strip().split(" ")
    for num in line:
        v.append(float(num))
    
    return v, end+1

if len(sys.argv) < 2:
    print("Missing command line param:\n    Usage: polysoup <map-name.map>")
    exit()

mapfile = open(sys.argv[1], "r")
mapfileLines = mapfile.readlines()
g_Callstack = []
g_TokenList = []

# for line in mapfileLines:
#     l = line.rstrip()
#     if getNextToken(l) == TokenType.COMMENT:
#         print(l)

def start(startLineno):
    lineno = startLineno
    currentLine = mapfileLines[lineno]

    token = getNextToken(currentLine.strip())
    while token.type != TokenType.NONE:
        g_TokenList.append(Token(token))
        lineno += 1
        if lineno < len(mapfileLines):
            currentLine = mapfileLines[lineno]
            token = getNextToken(currentLine)
            if token.type == TokenType.VERTEX:
                v0, cursorPos = getVertex(currentLine.strip())
                v1, cursorPos = getVertex(currentLine.strip()[cursorPos:])
                v2, cursorPos = getVertex(currentLine.strip()[cursorPos:])
                print(v0, v1, v2)

        else:
            token = Token(TokenType.NONE)


start(0)
# print(g_TokenList)

