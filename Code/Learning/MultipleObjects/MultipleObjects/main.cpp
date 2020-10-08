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
#include <stack>
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
stack < glm::mat4 > mvStack;

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

void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight) {
	// build perspective matrix
	aspect = (float)width / (float)height;
	glViewport(0,0,newWidth,newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void init(GLFWwindow* window) {

	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 0.0f, cameraY = 0.0f, cameraZ = 8.0f;
	cubeLocX = 0.0f, cubeLocY = -2.0f, cubeLocZ = 0.0f;
	setupVerticies();

	// setup window size and pMatrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}


void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);

	// get uniform variables for MV and projection matricies
	GLuint mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	GLuint projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// Build View Matrix
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ)); // reverse the camera coordinates
	mvStack.push(vMat);

	// ---------------Build the Sun -----------------
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 1.0f));
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// adjust OpenGL setting to draw model
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mvStack.pop();
	// --------------Build the Earth---------------
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime)*4.0f,
										0.0f, cos((float)currentTime)*4.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f),  (float)currentTime, glm::vec3(0.0,1.0,0.0));
	

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mvStack.pop(); // remove rotation but keep scale of planet;
	// --------------Build the moon---------------
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime)*2.0f,
											cos((float)currentTime)*2.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	while(!mvStack.empty()) {
		mvStack.pop();
	}
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


	glfwSetWindowSizeCallback(window, window_reshape_callback);

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

