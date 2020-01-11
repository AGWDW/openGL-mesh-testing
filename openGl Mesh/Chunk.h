#pragma once
#include "Camera.h"
#include "constants.h"
#include <sstream>
class Chunk
{
public:
	glm::vec3 position;
	std::array<GLuint, CHUNK_VOLUME> blocks; // 0 - air, 1 - grass
	Chunk(glm::vec3 pos);
	void create();
	std::vector<Mesh::FaceMesh>& getMeshes() {
		return meshes;
	}
private:
	std::vector<Mesh::FaceMesh> meshes;
};

