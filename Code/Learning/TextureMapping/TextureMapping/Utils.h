#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <SOIL2/soil2.h>
using namespace std;

class Utils {
public:
	static string readShaderSource(const char* filePath);
	static GLuint createShaderProgram(const char* vShaderFile, const char* fShaderFile);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(int prog);
	static bool checkOpenGLError();
	static GLuint loadTexture(const char* textureImagePath);
};

/*
*Read a Shader in from a file and return the file content as a string
*/
string Utils::readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	// while not at the end of the file
	// build line by line
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}


// create a shader program given cShaderFile and fShaderFile
GLuint Utils::createShaderProgram(const char* vShaderFile, const char* fShaderFile) {

	string vertShaderStr = readShaderSource(vShaderFile);
	string fragShaderStr = readShaderSource(fShaderFile);

	const char* vshaderSource = vertShaderStr.c_str();
	const char* fshaderSource = fragShaderStr.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);
	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;
}

/* ================utilities for debugging below===================*/

/*
	print shader log for GLuint shader
*/
void Utils::printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log:" << log << endl;
		free(log);
	}
}

/*
	print program log for a shader program
*/
void Utils::printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log:" << log << endl;
		free(log);
	}
}

/*
	check for any openGL errors
*/
bool Utils::checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError:" << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

/*Load a from a file and return the texture id for openGL*/
GLuint Utils::loadTexture(const char* textureImagePath) {
	GLuint textureId;
	textureId = SOIL_load_OGL_texture(textureImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureId == 0) {
		cout << "COULD NOT FIND TEXTURE IMAGE FILE" << textureImagePath << endl;
	}
	return textureId;
}
