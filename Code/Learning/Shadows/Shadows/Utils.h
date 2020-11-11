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
	// gold light functions
	static float* goldAmbient() { static float a[4] = {0.24734f, 0.1995f, 0.0745f, 1}; return (float*)a; }
	static float* goldDiffuse() { static float a[4] = { 0.7516f, 0.6065f, 0.2265f, 1 }; return (float*)a; }
	static float* goldSpecular() { static float a[4] = { 0.6283f, 0.5559f, 0.3661f, 1 }; return (float*)a; }
	static float goldShininess() { return 51.2f; }
	// silver light functions
	static float* silverAmbient() { static float a[4] = { 0.1923f, 0.1923f, 0.1932f, 1 }; return (float*)a; }
	static float* silverDiffuse() { static float a[4] = { 0.5705f, 0.5705f, 0.5705f, 1 }; return (float*)a; }
	static float* silverSpecular() { static float a[4] = { 0.5083f, 0.5083f, 0.5083f, 1 }; return (float*)a; }
	static float silverShininess() { return 51.2f; }
	// bronze light functions
	static float* bronzeAmbient() { static float a[4] = { 0.2125f, 0.1275f, 0.0540f, 1 }; return (float*)a; }
	static float* bronzeDiffuse() { static float a[4] = { 0.7140f, 0.4284f, 0.1814f, 1 }; return (float*)a; }
	static float* bronzeSpecular() { static float a[4] = { 0.3936f, 0.2719f, 0.1667f, 1 }; return (float*)a; }
	static float bronzeShininess() { return 25.6f; }
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

	// debugging setup
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource(vShaderFile);
	string fragShaderStr = readShaderSource(fShaderFile);

	const char* vshaderSource = vertShaderStr.c_str();
	const char* fshaderSource = fragShaderStr.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);
	
	// compile and check vertex shader for errors
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}

	// compile and check fragment shader for errors
	glCompileShader(fShader);
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	// link vertex and fragment shader and check for errors
	glLinkProgram(vfProgram);
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printShaderLog(vfProgram);
	}

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
