#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>


using namespace std;

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];


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



GLuint createShaderProgram() {

	string vertShaderStr = readShaderSource("vertShader.glsl");
	string fragShaderStr = readShaderSource("fragShader.glsl");

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
	renderingProgram = createShaderProgram();
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window, double currentTime) {
	glUseProgram(renderingProgram);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, 1);
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
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}

