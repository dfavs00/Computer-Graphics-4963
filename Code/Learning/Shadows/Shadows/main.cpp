#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Torus.h"
#include "Utils.h"
#include "ImportedModel.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 7

float cameraX, cameraY, cameraZ;
float torLocX, torLocY, torLocZ;
GLuint renderingProgram;
GLuint renderingProgram1;
GLuint renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

Torus myTorus(0.6f, 0.4f, 48);
int numTorusVertices = myTorus.getNumVertices();
int numTorusIndices = myTorus.getNumIndices();

ImportedModel pyramid("cone.obj");
int numPyramidVertices = pyramid.getNumVertices();

glm::vec3 torusLoc = glm::vec3(1.6, 0.0f, -0.3f);
glm::vec3 cameraLoc(0.0f, 0.2f, 6.0f);
glm::vec3 pyrLoc(-1.0f, 0.1f, 0.3f);
glm::vec3 lightLoc = glm::vec3(-3.8f, 2.2f, 1.1f);




float amt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc, sLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, transformed;
float lightPos[3];

// white light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// gold material for the pyramid
float* goldMatAmb = Utils::goldAmbient();
float* goldMatDif = Utils::goldDiffuse();
float* goldMatSpe = Utils::goldSpecular();
float goldMatShi = Utils::goldShininess();
// bronze material for the torus
float* bronzeMatAmb = Utils::bronzeAmbient();
float* bronzeMatDif = Utils::bronzeDiffuse();
float* bronzeMatSpe = Utils::bronzeSpecular();
float bronzeMatShi = Utils::bronzeShininess();

// variables used in display() for transfering light 
float curAmb[4], curDif[4], curSpe[4], matAmb[4], matDif[4], matSpe[4];
float curShi, matShi;

// shadow-related variables
int screenSizeX, screenSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

void installLights(GLuint renderingProgram, glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}

void setupVertices(void) {
	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	std::vector<glm::vec2> tex = myTorus.getTexCoords();
	std::vector<glm::vec3> norm = myTorus.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myTorus.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
	//----------------------------------model-------------------------------------
	vector<glm::vec3> vertM = pyramid.getVertices();
	vector<glm::vec2> texM = pyramid.getTextureCoords();
	vector<glm::vec3> normM = pyramid.getNormals();
	int numObjVertices = pyramid.getNumVertices();

	vector<float> pvaluesM;	// vertex
	vector<float> tvaluesM;	// texture
	vector<float> nvaluesM;	// normal

	//cout << vertM.size() << endl;

	// populate buffer arrays
	for (int i = 0; i < numObjVertices; i++) {
		pvaluesM.push_back((vertM[i]).x);
		pvaluesM.push_back((vertM[i]).y);
		pvaluesM.push_back((vertM[i]).z);

		tvaluesM.push_back((texM[i]).s);
		tvaluesM.push_back((texM[i]).t);

		nvaluesM.push_back((normM[i]).x);
		nvaluesM.push_back((normM[i]).y);
		nvaluesM.push_back((normM[i]).z);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, pvaluesM.size() * 4, &pvaluesM[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, tvaluesM.size() * 4, &tvaluesM[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, nvaluesM.size() * 4, &nvaluesM[0], GL_STATIC_DRAW);
}


void setupShadowBuffers(GLFWwindow* window) {
	glfwGetFramebufferSize(window, &width, &height);
	screenSizeX = width;
	screenSizeY = height;
	// create the custom frame buffer
	glGenFramebuffers(1, &shadowBuffer);
	// create the shadow texture and configure it to hold depth information.
	// these steps are similar to those in Program 5.2
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}


void init(GLFWwindow* window) {
	cout << "creating shader programs" << endl;
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	renderingProgram1 = Utils::createShaderProgram("vert1Shader.glsl", "frag1Shader.glsl");
	renderingProgram2 = Utils::createShaderProgram("vert2Shader.glsl", "frag2Shader.glsl");
	cout << "done." << endl;

	setupVertices();
	setupShadowBuffers(window);

	b = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);
}


void passOne(void) {
	// renderingProgram1 includes the pass one vertex and fragment shaders
	glUseProgram(renderingProgram1);

	// the following blocks of code render the torus to establish the depth buffer from the light point of view
	mMat = glm::translate(glm::mat4(1.0f), torusLoc);

	// slight rotation for viewability
	mMat = glm::rotate(mMat, toRadians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// we are drawing from the light’s point of view, so we use the light’s P and V matrices
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP1");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	// we only need to set up torus vertices buffer – we don’t need its textures or normals for pass one.
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // vbo[3] contains torus indices
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);


	// repeat for the pyramid (but don’t clear the GL_DEPTH_BUFFER_BIT)
	// The pyramid is not indexed, so we use glDrawArrays() rather than glDrawElements()
	mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP1");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}


void passTwo(void) {
	glUseProgram(renderingProgram2); // pass two vertex and fragment shaders
	// draw the torus – this time we need to include lighting, materials, normals, etc.
	// We also need to provide MVP tranforms for BOTH camera space and light space.
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP2");
	// the torus is bronze
	curAmb[0] = bronzeMatAmb[0]; curAmb[1] = bronzeMatAmb[1]; curAmb[2] = bronzeMatAmb[2];
	curDif[0] = bronzeMatDif[0]; curDif[1] = bronzeMatDif[1]; curDif[2] = bronzeMatDif[2];
	curSpe[0] = bronzeMatSpe[0]; curSpe[1] = bronzeMatSpe[1]; curSpe[2] = bronzeMatSpe[2];
	curShi = bronzeMatShi;


	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));
	currentLightPos = glm::vec3(lightLoc);
	installLights(renderingProgram2, vMat);

	mMat = glm::translate(glm::mat4(1.0f), torusLoc);

	// slight rotation for viewability
	mMat = glm::rotate(mMat, toRadians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// build the MV matrix for the torus from the camera’s point of view
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	// build the MVP matrix for the torus from the light’s point of view
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	// put the MV and PROJ matrices into the corresponding uniforms
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	// set up torus vertices and normals buffers (and texture coordinates buffer if used)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // torus vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // torus normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // vbo[3] contains torus indices
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
	
	
	// repeat for the gold pyramid
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// set up view and perspective matrix from the light point of view, for pass 1
	lightVmatrix = glm::lookAt(currentLightPos, glm::vec3(0,0,0), glm::vec3(0,1,0)); // vector from light to origin
	lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

	// make the custom frame buffer current, and associate it with the shadow texture
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

	// disable drawing colors, but enable the depth computation
	//glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	passOne();
	
	// restore the default display buffer, and re-enable drawing
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	//glDrawBuffer(GL_FRONT); // re-enables drawing colors
	//passTwo();
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {


	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter7 - program1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	
	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);
	

	while (!glfwWindowShouldClose(window)) {
		
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}