#pragma once
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Mesh.h"
#include "ShaderLoader.h"
#include "Camera.h"

class LightRenderer
{
public:
	LightRenderer(MeshType meshType, Camera* camera);
	~LightRenderer();
	void draw();
	void setPosition(glm::vec3 _position);
	void setColor(glm::vec3 _color);
	void setProgram(GLuint program);
	glm::vec3 getPosition();
	glm::vec3 getColor();

private:
	Camera* camera;
	std::vector<Vertex>vertices;
	std::vector<GLuint>indices;
	glm::vec3 position, color;
	/// <summary>
	/// This is the geometrical information; it includes attributes such as position, color,
	/// normal, and texture coordinates – these are stored on a per vertex basis on the GPU.
	/// </summary>
	GLuint vbo;
	/// <summary>
	/// This is used to store the index of each vertex and will be used while drawing the mesh.
	/// </summary>
	GLuint ebo;
	/// <summary>
	/// This is a helper container object that stores all the VBOs and attributes.
	/// This is because you might have more than one one VBO per object,
	/// and it will be tedious to bind the VBOs all over again when you render each frame.
	/// </summary>
	GLuint vao;
	GLuint program;

};
