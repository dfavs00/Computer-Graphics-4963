#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <stack>

/*Containter class that stores information about an object*/
class Object {
private:
	//Object* parent; // points to parent object
	//Object* child;  // points to child object
	glm::vec3* position;
	glm::vec3* rotation;
	glm::vec3* scale;
	int vertexBufferIndex;
	int textureBufferIndex;
	int objectIndex; // family index that this object belongs to
	int numVerticies; // number of vertices this object contains

public: 
	// accessors
	//Object getParent() { return *parent; }
	//Object getChild() { return *child; }
	int getVBuffer() { return vertexBufferIndex; }
	int getTBuffer() { return textureBufferIndex; }
	int getObjectIndex() { return objectIndex; }
	int getNumVertices() { return numVerticies; }

	// i am returning references for ease of ImGui InputFloat3... be careful when changing these values
	glm::vec3* getPosition() { return position; }
	glm::vec3* getRotation() { return rotation; }
	glm::vec3* getScale() { return scale; }


	Object(int vBuff, int tBuff, int oIndex, int numVert) { 
		position = new glm::vec3(0); rotation = new glm::vec3(0); scale = new glm::vec3(1);
		vertexBufferIndex = vBuff; textureBufferIndex = tBuff;
		objectIndex = oIndex;
		numVerticies = numVert;
	}

	// set functions
	void setPosition(glm::vec3 pos) { *position = pos; }
	void setRotation(glm::vec3 rot) { *rotation = rot; }
	void setScale(glm::vec3 sca) { *scale = sca; }
};

/*class that stores and handles a collection of objects*/
class Collection {

public:
	std::vector<Object*> objects;
	void addChildObject(Object* o) { objects.push_back(o); }
	void removeLastChild() { objects.pop_back(); }
	inline void drawThisCollection(std::stack<glm::mat4> *mvStack, GLuint mvLocation, GLuint projLocation, GLuint* vbo);
	int size() { return objects.size(); }
};


/*draw this collection by looping through each object in the collection*/
inline void Collection::drawThisCollection(std::stack<glm::mat4> *mvStack, GLuint mvLocation, GLuint projLocation, GLuint* vbo) {
	
	//loop through each object in the collection and draw each object
	for (int i = 0; i < objects.size(); i++) {

		Object* current = objects[i];

		//-----------position
		mvStack->push(mvStack->top());
		mvStack->top() *= glm::translate(glm::mat4(1.0f), *(current->getPosition()));
		//-----------rotation
		mvStack->push(mvStack->top());
		mvStack->top() *= glm::rotate(glm::mat4(1.0f), (current->getRotation())->x, glm::vec3(1.0f, 0.0f, 0.0f));
		mvStack->push(mvStack->top());
		mvStack->top() *= glm::rotate(glm::mat4(1.0f), (current->getRotation())->y, glm::vec3(0.0f, 1.0f, 0.0f));
		mvStack->push(mvStack->top());
		mvStack->top() *= glm::rotate(glm::mat4(1.0f), (current->getRotation())->z, glm::vec3(0.0f, 0.0f, 1.0f));
		// ----------scale
		mvStack->push(mvStack->top());
		mvStack->top() *= glm::scale(glm::mat4(1.0f), *(current->getScale()));

		// ----------give mvLocation in the shader the stack information
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(mvStack->top()));

		// ----------draw object
		glBindBuffer(GL_ARRAY_BUFFER, vbo[current->getVBuffer()]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[current->getTBuffer()]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, current->getNumVertices());
	}

	// ---------clear this collection's stack
	while (!mvStack->empty()) {
		mvStack->pop();
	}
}