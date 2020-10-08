#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variables used in display to save time during rendering
GLuint myLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;

// Eventually might be able to pass a file into here or something
void setupVerticies(void) {
	float vertexPositions[108] = {
		-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}



void init(GLFWwindow* window) {

	renderingProgram =  Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 0.0f, cameraY = 0.0f, cameraZ = 8.0f;
	cubeLocX = 0.0f, cubeLocY = -2.0f, cubeLocZ = 0.0f;
	setupVerticies();
}


void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	
	tMat = glm::translate(glm::mat4(1.0f),
		glm::vec3(sin(0.35f*currentTime)*2.0f, 
		cos(0.52f*currentTime)*2.0f,
		sin(0.7f*currentTime)*2.0f ));
	rMat = glm::rotate(glm::mat4(1.0f), 1.75f*(float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	rMat = glm::rotate(rMat, 1.75f*(float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
	rMat = glm::rotate(rMat, 1.75f*(float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
	mMat = tMat * rMat;

	//Utils::checkOpenGLError();

	// get uniform variables for MV and projection matricies
	GLuint mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	GLuint projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	
	// build perspective matrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//angle is in radians (60.0f degrees == 1.0472f radians)
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	
	// Build View Matrix
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ) ); // reverse the camera coordinates
	mvMat = vMat * mMat;

	//put matricies into correct uniform variables in rendering program
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// Vertex Buffer Object gets associated with the correct vertex 
	// attribute in the vertex shader
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// adjust OpenGL setting to draw model
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


int main(void) {

	//glfwInit initializes the GLFW library
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//Tells the program what GLFW version that is being used (in this case 4.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Make window
	GLFWwindow* window = glfwCreateWindow(600, 600, "Cube", NULL, NULL);
	glfwMakeContextCurrent(window);

	//glewInit initializes the GLEW library
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init(window);

	//Loop runs until the window is closed
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}

