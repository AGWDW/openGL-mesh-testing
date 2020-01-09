#pragma once
#include "SOIL2/SOIL2.h"
#include "Mesh.h"
#include "Shader.h"
#include "Entity.h"
#include "Camera.h"
#include "Buffer.h"
#include <gtc/matrix_transform.hpp>
class MeshRender {
public:
	glm::vec3 position;
	Mesh mesh;
	//GLuint VBO, VAO;
	Buffer buffers;
	MeshRender();
	MeshRender(glm::vec3 pos);
	void loadMesh(Mesh& m);
	void create();
	void render(Camera p1, glm::mat4 projection);
	void destroy();
	void loadStruct(Structure& data);
private:
	std::array<GLuint, 2> texMaps;
	Shader shader;
	void loadTexture(std::string name);
	void loadTexmap(std::string name);
};