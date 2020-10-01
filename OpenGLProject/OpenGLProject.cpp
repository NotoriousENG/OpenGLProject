// OpenGLProject.cpp : Defines the entry point for the application.
//

#include "OpenGLProject.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "ShaderLoader.h"
#include "Camera.h"
#include "LightRenderer.h"

Camera* camera;
LightRenderer* light;

static void glfwError(int id, const char* description);
void initGame();
void renderScene();

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

void initGame()
{
	glEnable(GL_DEPTH_TEST);

	ShaderLoader shader;

	GLuint flatShaderProgram = 
		shader.createProgram(
			"Assets/Shaders/FlatModel.vert", 
			"Assets/Shaders/FlatModel.frag");

	camera = new Camera(45.0f, 800, 600, 0.1f, 100.0f, glm::vec3(0.0f,4.0f, 6.0f));

	light = new LightRenderer(MeshType::kCube, camera);
	light->setProgram(flatShaderProgram);
	light->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 0, 1);

	// draw game objects here
	light->draw();
}


int main(int argc, char **argv)
{
	glfwSetErrorCallback(&glfwError);
	
	glfwInit();
	auto* window = glfwCreateWindow(800, 600, "Hello OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewInit();
	initGame();
	while (!glfwWindowShouldClose(window))
	{
		// render our scene
		renderScene();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	delete camera;
	delete light;
	
	return 0;
}
