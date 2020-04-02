#pragma once
#include <gtx/string_cast.hpp>
#include <algorithm>
// #include "../World/World.h"
#include "../../Renders/chunkRender.h"
#include "../Physics/BoxCollider.h"
#include "../World/Chunks/ChunkColumn.h"
// #include "../World/World.h"
using Face = std::tuple<Buffer*, Texture*, glm::vec3>;
class Entity
{
public:
	Entity(GLboolean init = GL_FALSE, glm::vec3 dimentions = glm::vec3(1.0f));
	glm::vec3& getPosition();
	glm::vec3& getVelocity();
	glm::vec3& getAcceleration();
	glm::vec3 getCenter();
	glm::vec3 getCenter(glm::vec3 pos);
	BoxCollider& getCollider();
	Faces& getBody();
	
	void setPosition(const glm::vec3& pos);
	void setVelocity(const glm::vec3& vel);
	void setVelocity(const GLfloat& vel);
	void setAcceleration(const glm::vec3& acc);
	void setMovementSpeed(const GLfloat& speed);
	void setTextues(Texture_Names texB, Texture_Names texT);
	void setTarget(glm::vec3 targ);
	void getNewTarget();

	void addVelocity(const glm::vec3& vel);
	void addAcceleration(const glm::vec3& acc);

	void updatePosition(GLfloat deltaTime, std::vector<ChunkColumn*>& adjacesnt, std::string& collision);

	void move(Move_Dir dir);
	void moveToTarget();
	void create();

	void render(glm::mat4 projection, Camera* cam = nullptr);

	void takeDamage(GLuint dmgTaken);
	void update();
	GLboolean isDead;
protected:
	glm::ivec3 determinCollision(std::vector<ChunkColumn*>& adjacesnt, glm::vec3 deltaV);
	void calcMovementPath();
	void moveBlock(Move_Dir dir);
	void lookAt(Move_Dir dir);
	void toggleShowDamage();
	void checkDead();

	glm::vec3 pos, vel, acc;
	glm::vec3 forward, right;
	GLfloat movementSpeed;
	GLfloat jumpForce;
	std::vector<Face> body;
	Render::ChunkMeshRender renderer;
	GLboolean grounded;
	GLboolean hasBody;
	BoxCollider collider;
	Texture_Names textures[2];

	glm::vec3 targetPosition, prevBlock, nxtBlock;
	GLboolean hasTarget;
	GLuint stage;
	std::vector<Move_Dir> movementPath;

	GLboolean showDmg;
	/*const*/ GLubyte dmgCooldown; // in frames
	GLubyte dmgTimer;
	GLboolean invunrable;
	GLubyte invunrableTimer;
	/*const*/ GLubyte invunrableCooldown; // in frames

	GLint health;
	GLuint attackDmg;
};

