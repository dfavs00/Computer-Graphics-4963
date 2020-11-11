#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Sphere.h"
using namespace std;

Sphere::Sphere() {
	init(48);
}

// number of precision slices
Sphere::Sphere(int prec) {
	init(prec);
}

// convert an angle in degrees to radians
float Sphere::toRadians(float degrees) {
	return (degrees * 2.0f * 3.14159f) / 360.0f;
}

// create the actual sphere object
void Sphere::init(int prec) {
	numVertices = (prec + 1) * (prec + 1);
	numIndicies = prec * prec * 6;
	// populate the data structure with empty values
	for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
	for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); } 
	for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
	for (int i = 0; i < numIndicies; i++) { indicies.push_back(0); }

	// calculate the verticies of each triangle
	for (int i = 0; i <= prec; i++) {
		for (int j = 0; j <= prec; j++) {
			float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
			float x = -(float)cos(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
		
			vertices[i*(prec + 1) + j] = glm::vec3(x, y, z);
			texCoords[i*(prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
			normals[i*(prec + 1) + j] = glm::vec3(x, y, z);
		}
	}

	//calculate the indicies of each triangle
	for (int i = 0; i < prec; i++) {
		for (int j = 0; j < prec; j++) {
			indicies[6 * (i*prec + j) + 0] = i*(prec + 1) + j;
			indicies[6 * (i*prec + j) + 1] = i*(prec + 1) + j + 1;
			indicies[6 * (i*prec + j) + 2] = (i+1)*(prec + 1) + j;
			indicies[6 * (i*prec + j) + 3] = i*(prec + 1) + j + 1;
			indicies[6 * (i*prec + j) + 4] = (i+1)*(prec + 1) + j + 1;
			indicies[6 * (i*prec + j) + 5] = (i+1)*(prec + 1) + j;
		}
	}
}

// Accessor Functions
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndicies() { return numIndicies; }
std::vector<int> Sphere::getIndicies() { return indicies; }
std::vector<glm::vec3> Sphere::getVertices() { return vertices; }
std::vector<glm::vec2> Sphere::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Sphere::getNormals() { return normals; }

