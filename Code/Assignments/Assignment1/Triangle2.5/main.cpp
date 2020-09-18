#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>


using namespace std;

#define numVAOs 1

GLuint renderingProgram;
GLuint renderingProgram2;
GLuint vao[numVAOs];

//test
// hardcoded framerate
double frameRate = 540;
double lastTimeRun;
double renderTime;

/*
 *Read a Shader in from a file and return the file content as a string
 */
string readShaderSource(const char* filePath) {
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


// book reccomends putting this into a utilities file since it will probably be used a lot
GLuint createShaderProgram(const char* vShaderFile, const char* fShaderFile) {
	
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



void init(GLFWwindow* window) {

	renderingProgram = createShaderProgram("vertShader.glsl", "fragShader.glsl");
	renderingProgram2 = createShaderProgram("vertShader2.glsl", "fragShader2.glsl");
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);

	renderTime = lastTimeRun = glfwGetTime();
}


// Globals
float x_pos = 0.0f;
float increment = 0.0001f;


void clear() {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}


void display(GLFWwindow* window, double currentTime) {

	// might be better to render on a different thread to get framerate consistent?

	// attempt at limiting the framerate...
	double delta = currentTime - lastTimeRun;
	if (delta <  ((double)1 / frameRate) ) {
		//cout << delta << endl;
		return;
	}
	// get beginning of render time

	clear();
	//=============== First rendering program===========================
	glUseProgram(renderingProgram);
	// uint representing location of "offset" in renderingProgram
	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
	// put x_pos into offsetLoc position in renderingProgram
	glProgramUniform1f(renderingProgram, offsetLoc, x_pos);
	// Color change for the fragment shader
	GLuint colorChangeLoc = glGetUniformLocation(renderingProgram, "change");
	glProgramUniform1f(renderingProgram, colorChangeLoc, x_pos);
	// extra triangle
	// just hard coded triangles in not exactly sure how to look at the bigger picture of
	// building complex geometric shapes and models with these triangles
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//================Second Rendering Program=========================
	glUseProgram(renderingProgram2);
	colorChangeLoc = glGetUniformLocation(renderingProgram2, "change");
	glProgramUniform1f(renderingProgram2, colorChangeLoc, x_pos);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// more framerate stuff
	lastTimeRun = glfwGetTime();
}

int main(void) {

	//glfwInit initializes the GLFW library
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//Tells the program what GLFW version that is being used (in this case 4.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Make window
	GLFWwindow* window = glfwCreateWindow(600, 600, "TEST WINDOW", NULL, NULL);
	glfwMakeContextCurrent(window);

	//glewInit initializes the GLEW library
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init(window);

	//Loop runs until the window is closed
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());

		// making the position change no matter what
		x_pos += increment;
		if (x_pos > 0.5f)
			increment = -0.0001f;
		if (x_pos < 0.0f)
			increment = 0.0001f;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}

