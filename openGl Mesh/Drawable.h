#pragma once
#include <chrono>
#include "Game/World/constants.h"
#include "Game/Player/Camera.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
using FaceB = std::tuple<Buffer, Texture*, GLuint>;
class Drawable
{
public:
	Drawable();
	Drawable(std::vector<Face*>& sortedMeshes);
	void render(Camera& cam, glm::mat4 projection);
	void setUp(std::vector<Face*>& sortedMeshes);
	void setUp(std::vector<Face*> sortedMeshes, const char* overload);
private:
	std::vector<Face*> meshes;
	std::map<GLuint, FaceB> buffers;
};
