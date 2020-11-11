#include "ImportedModel.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;

//-------------------ImportedModel--------------------
ImportedModel::ImportedModel(const char* filepath) {
	ModelImporter modelImporter = ModelImporter();
	modelImporter.parseOBJ(filepath); // get vertex information
	numVertices = modelImporter.getNumVertices();
	vector<float> verts = modelImporter.getVertices();
	vector<float> tcs = modelImporter.getTextureCoordinates();
	vector<float> normals = modelImporter.getNormals();

	// populate member variables with vertex data
	for (int i = 0; i < numVertices; i++) {
		vertices.push_back(glm::vec3(verts[i*3], verts[i*3+1], verts[i*3+2]));
		texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normalVecs.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
}
//-----------------ModelImporter---------------------
ModelImporter::ModelImporter() {}

void ModelImporter::parseOBJ(const char* filepath) {
	float x, y, z;
	string content;
	ifstream fileStream(filepath, ios::in);
	if (!fileStream.good()) {
		cout << "Could not open File \"" << filepath << "\"" << endl;
	}
	string line = "";

	// loop until the end of the file
	while (!fileStream.eof()) {
		getline(fileStream, line);
		//cout << line << endl;
		// vertex position
		if (line.compare(0, 2, "v ") == 0) {
			//cout << "here" << endl;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
			//cout << x << ", " << y << ", " << z << endl;
		}
		// texture coordinates
		if (line.compare(0, 2, "vt") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		// vertex normal positions
		if (line.compare(0, 2, "vn") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		//triangle face positions
		if (line.compare(0, 2, "f ") == 0) {

			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			//cout << ss.str() << endl;
			for (int i = 0; i < 3; i++) {
				getline(ss, oneCorner, ' ');
				//cout << oneCorner << endl;

				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');
				
				
				int vertRef = (stoi(v) - 1) * 3;
				int tcRef = (stoi(t) - 1) * 2;
				int normRef = (stoi(n) - 1) * 3;

				triangleVerts.push_back(vertVals[vertRef]);
				triangleVerts.push_back(vertVals[vertRef+1]);
				triangleVerts.push_back(vertVals[vertRef+2]);

				textureCoords.push_back(stVals[tcRef]);
				textureCoords.push_back(stVals[tcRef+1]);

				normals.push_back(normVals[normRef]);
				normals.push_back(normVals[normRef+1]);
				normals.push_back(normVals[normRef+2]);
			}
			
		}
	}
}
