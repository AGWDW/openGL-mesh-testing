#pragma once
#include <sstream>
#include "../../Mesh.h"

#define RENDER_DISTANCE 2
#define CHUNK_SIZE 4
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

#define GRAVITY 9.81
#define AIR_RESISTANCE 2

using namespace Mesh;
enum FACES_NAMES {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};
enum TEXTURE_NAMES {
	GRASS
};

static std::array<Buffer*, 6> FACES = {
	FaceMesh({ glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5) },   // FRONT
	{ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) },
	{ glm::vec3(-1, -1, 1), glm::vec3(-1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5) },  // BACK
	{ glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec3(0, 0, -1) },
	{ glm::vec3(-1, 1, -1), glm::vec3(-1), glm::vec3(1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-0.5), glm::vec3(-0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5, 0.5, 0.5) },  // LEFT
	{ glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0) },
	{ glm::vec3(1, -1, -1), glm::vec3(1, -1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1) }).getBuffer(),

	FaceMesh({ glm::vec3(0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5) },     // RIGHT
	{ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0) },
	{ glm::vec3(-1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(1), glm::vec3(1), glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0.5, 0.5, -0.5),glm::vec3(0.5), glm::vec3(0.5), glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-0.5, 0.5, -0.5) },	  // TOP
	{ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) },
	{ glm::vec3(-1), glm::vec3(-1, -1, 1), glm::vec3(1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1) }).getBuffer(),

	FaceMesh({ glm::vec3(-0.5), glm::vec3(0.5, -0.5, -0.5),glm::vec3(0.5, -0.5, 0.5), glm::vec3(0.5, -0.5, 0.5), glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-0.5) },   // BOTTOM
	{ glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0), glm::vec3(0, -1, 0) },
	{ glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1), glm::vec3(1), glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1) }).getBuffer()
};
static std::vector<Texture*> TEXTURES = {
	new Texture("grass")
};

GLuint getBlockIndex(glm::vec3 position);
