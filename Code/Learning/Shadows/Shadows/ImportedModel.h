#include <vector>
#include <glm/glm.hpp>
using namespace std;

class ImportedModel {
private:
	int numVertices;
	vector<glm::vec3> vertices;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> normalVecs;

public:
	ImportedModel(const char* filepath);
	int getNumVertices() { return numVertices; }
	vector<glm::vec3> getVertices() { return vertices; }
	vector<glm::vec2> getTextureCoords() { return texCoords; }
	vector<glm::vec3> getNormals() { return normalVecs; }
};

class ModelImporter {
private:
	vector<float> vertVals;
	vector<float> stVals;
	vector<float> normVals;
	// for vertex attributes
	vector<float> triangleVerts;
	vector<float> textureCoords;
	vector<float> normals;

public:
	ModelImporter();
	void parseOBJ(const char* filepath);
	int getNumVertices() { return triangleVerts.size() / 3; };
	vector<float> getVertices() { return triangleVerts; }
	vector<float> getTextureCoordinates() { return textureCoords; }
	vector<float> getNormals() { return normals; }
};