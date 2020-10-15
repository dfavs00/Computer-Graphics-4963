#include <stdlib.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <stack>
#include <SOIL2/SOIL2.h>
#include "Utils.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "ImportedModel.h"
#include "Sphere.h"
#include "Object.cpp"

//  About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.


//#include <GL/glew.h>            // Initialize with glewInit()


// Include glfw3.h after our OpenGL definitions
//#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

using namespace std;


//------------------------variables
#define numVAOs 1
#define numVBOs 8

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
stack < glm::mat4 > mvStack;
GLuint texture;

// variables used in display to save time during rendering
GLuint myLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;
//ImVec4 clear_color;

// create a sphere
Sphere sphere(48);

// shuttle object
ImportedModel model("shuttle.obj");

// creating collections of objects
//vector<Collection> objectCollections;
Collection collection;
//Object* newObj;

vector<glm::vec3> collectionPositions;
vector<glm::vec3> collectionRotations;
vector<glm::vec3> collectionScales;
//------------------------

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

	float squareTextCoords[72] = {
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	};

	// ------------------sphere-----------------
	vector<int> ind = sphere.getIndicies();
	vector<glm::vec3> vert = sphere.getVertices();
	vector<glm::vec2> tex = sphere.getTexCoords();
	vector<glm::vec3> norm = sphere.getNormals();

	vector<float> pvalues;	// vertex
	vector<float> tvalues;	// texture
	vector<float> nvalues;	// normal


	// populate buffer arrays
	int numIndicies = sphere.getNumIndicies();
	for (int i = 0; i < numIndicies; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);

		tvalues.push_back((tex[ind[i]]).x);
		tvalues.push_back((tex[ind[i]]).y);

		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}
	// --------------shuttle---------------
	vector<glm::vec3> vertM = model.getVertices();
	vector<glm::vec2> texM = model.getTextureCoords();
	vector<glm::vec3> normM = model.getNormals();
	int numObjVertices = model.getNumVertices();

	vector<float> pvaluesM;	// vertex
	vector<float> tvaluesM;	// texture
	vector<float> nvaluesM;	// normal

	cout << vertM.size() << endl;

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

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	//-----------------Cube------------------
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareTextCoords), squareTextCoords, GL_STATIC_DRAW);

	//-----------------Sphere----------------
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	//----------------Shuttle----------------
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, pvaluesM.size() * 4, &pvaluesM[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, tvaluesM.size() * 4, &tvaluesM[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, nvaluesM.size() * 4, &nvaluesM[0], GL_STATIC_DRAW);
}


void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight) {
	// build perspective matrix
	aspect = (float)width / (float)height;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}


void init(GLFWwindow* window) {

	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	cameraX = 0.0f, cameraY = 0.0f, cameraZ = 8.0f;
	setupVerticies();

	// setup window size and pMatrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	// load texture file
	texture = Utils::loadTexture("cubeTexture.jpg");
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


	collection.drawThisCollection(&mvStack, mvLoc, projLoc, vbo);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


void createObjectWidget(Object* obj, const char* name) {
	//cout << collection.objects[i] << endl;
	if (ImGui::CollapsingHeader(name)) {
		ImGui::InputFloat3("Position", (float*)(obj->getPosition()));
		ImGui::InputFloat3("Rotation", (float*)(obj->getRotation()));
		ImGui::InputFloat3("Scale   ", (float*)(obj->getScale()));
	}
}


// draw the objects in imgui in a format
void drawObjectCollections() {

	if (ImGui::Button("Add Cube")){
		// add a child to the collection with cubes dat
		Object* newObj =  new Object(0, 1, 0, 108);
		collection.addChildObject(newObj);
		collectionPositions.push_back(glm::vec3(0, 0, 0));
		collectionRotations.push_back(glm::vec3(0, 0, 0));
		collectionScales.push_back(glm::vec3(0, 0, 0));
	}
	if (ImGui::Button("Add Sphere")) {
		// add a child to the collection with spheres
		Object* newObj = new Object(2, 3, 0, sphere.getNumIndicies());
		collection.addChildObject(newObj);
		collectionPositions.push_back(glm::vec3(0, 0, 0));
		collectionRotations.push_back(glm::vec3(0, 0, 0));
		collectionScales.push_back(glm::vec3(0, 0, 0));
	}
	if (ImGui::Button("Add Rocket")) {
		// add a child to the collection with rockets data
		Object* newObj = new Object(5, 6, 0, model.getNumVertices());
		collection.addChildObject(newObj);
		collectionPositions.push_back(glm::vec3(0, 0, 0));
		collectionRotations.push_back(glm::vec3(0, 0, 0));
		collectionScales.push_back(glm::vec3(0, 0, 0));
	}

	
	//Loop through all objects in the collection and render them in ImGui
	for (int i = 0; i < collection.size(); i++) {
		//cout << collection.objects[i] << endl;

		stringstream ss;
		ss << "Object " << i << endl;
		string objId = ss.str();
		// push an id so that it is not thinking the same element has the same name
		ImGui::PushID(objId.c_str());
		createObjectWidget(collection.objects[i], objId.c_str());
		ImGui::PopID();

		//cout << "Object " << i << ": " << collection.objects[i]->getPosition()->x << endl;

		//cout << collection.objects[i]->getPosition().y << endl;

	}

	if(ImGui::Button("Remove Last Object")) {
		collection.removeLastChild();
	}

}


int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Assignment 2", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

	glfwSetWindowSizeCallback(window, window_reshape_callback);

	init(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop of the program
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

		display(window, glfwGetTime());


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


		//ImGui::ShowDemoWindow();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
			static float f = 0.0f;
			static float cameraXYZ[3] = {0.0f,0.0f,8.0f};
            ImGui::Begin("CONTROL PANEL");                          
 
			// Edit the background color
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			// setup the camera x y and z value
			ImGui::InputFloat3("Camera Position", cameraXYZ);

			
			//--------Render Contents of ObjectCollections--------
			drawObjectCollections();
			//----------------------------------------------------

			// set the camera X Y and Z values
			cameraX = cameraXYZ[0];
			cameraY = cameraXYZ[1];
			cameraZ = cameraXYZ[2];

			// print framerate of imgui frame
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

		

        // Rendering
        ImGui::Render();

        //int display_w, display_h;
        //glfwGetFramebufferSize(window, &display_w, &display_h);
        //glViewport(0, 0, display_w, display_h);
		
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
