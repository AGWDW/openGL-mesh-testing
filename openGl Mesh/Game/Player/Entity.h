#pragma once
#include "../World/constants.h"
#include "../../Renders/chunkRender.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Entity
{
public:
	Entity(GLboolean init = GL_FALSE);
	glm::vec3& getPosition();
	glm::vec3& getVelocity();
	glm::vec3& getAcceleration();
	glm::vec3 getCenter();
	
	void setVelocity(const glm::vec3& vel);
	void setVelocity(const GLfloat& vel);
	void setAcceleration(const glm::vec3& acc);

	void addVelocity(const glm::vec3& vel);
	void addAcceleration(const glm::vec3& acc);

	void updatePosition(GLfloat deltaTime, std::vector<Face*> ground);

	void move(Move_Dir dir);

	void render(glm::mat4 projection, Camera* cam = nullptr);
	GLboolean determinCollision(std::array<glm::vec3, 4> plane, glm::vec3 deltaV);
	GLboolean determinCollision(std::array<glm::vec3, 4> plane);

	void findGrounded(std::vector<Face*> ground, glm::vec3 deltaV);

protected:
	glm::vec3 pos, vel, acc;
	glm::vec3 forward, right;
	GLfloat movementSpeed;
	GLfloat jumpForce;
	std::vector<Face> body;
	Render::ChunkMeshRender renderer;
	GLboolean grounded;
	GLboolean hasBody;

};

