#include "Entity.h"
Entity::Entity(GLboolean init) {
	pos = { 0, 0, 0 };
	vel = { 0, 0, 0 };
	acc = { 0, 0, 0 };
	forward = { 0, 0, -1 };
	right = { 1, 0, 0 };
	movementSpeed = 1; // m/s
	jumpForce = 5;
	grounded = 0;
	hasBody = GL_FALSE;
	if (init) {
		renderer = Render::ChunkMeshRender(true, "");
	}
	else {
		renderer = Render::ChunkMeshRender(false, "");
	}
}
glm::vec3& Entity::getPosition() {
	return pos;
}
glm::vec3& Entity::getVelocity() {
	return vel;
}
glm::vec3& Entity::getAcceleration() {
	return acc;
}

void Entity::setVelocity(const glm::vec3& vel) {
	this->vel = vel;
}
void Entity::setVelocity(const GLfloat& vel) {
	this->vel = glm::vec3(vel);
}
void Entity::setAcceleration(const glm::vec3& acc) {
	this->acc = acc;
}
void Entity::setMovementSpeed(const GLfloat& speed) {
	movementSpeed = speed;
}

void Entity::addVelocity(const glm::vec3& vel) {
	this->vel += vel;
}
void Entity::addAcceleration(const glm::vec3& acc) {
	this->acc += acc;
}

void Entity::updatePosition(GLfloat deltaTime, World& world) {
	grounded = 0;
	vel += acc * deltaTime;
	glm::vec3 collisions = determinCollision(world, vel * deltaTime);
	if (collisions.y == -1) {
		grounded = 1;
	}
	if (collisions.x == 1) {
		if (vel.x > 0) vel.x = 0;
	}
	if (collisions.x == -1) {
		if (vel.x < 0) vel.x = 0;
	}
	if (collisions.z == 1) {
		if (vel.z > 0) vel.z = 0;
	}
	if (collisions.z == -1) {
		if (vel.z < 0) vel.z = 0;
	}

	if (!grounded) vel.y -= GRAVITY * deltaTime;
	else if (vel.y < 0) vel.y = 0;
	pos += vel * deltaTime;
	renderer.setPosition(pos);
}

void Entity::move(Move_Dir dir) {
	switch (dir)
	{
	case Move_Dir::FORWARD:
		vel += forward * movementSpeed;
		break;
	case Move_Dir::BACKWARD:
		vel += -forward * movementSpeed;
		break;
	case Move_Dir::LEFT:
		vel += -right * movementSpeed;
		break;
	case Move_Dir::RIGHT:
		vel += right * movementSpeed;
		break;
	case Move_Dir::UP:
		if (!grounded) break;
		vel.y += jumpForce;
		break;
	case Move_Dir::DOWN:
		break;
	}
}

void Entity::render(glm::mat4 projection, Camera* cam) {
	if (!hasBody) return;
	renderer.render(*cam, projection);
}
GLboolean isAbove(glm::vec3 above, glm::vec3 bellow, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { bellow + glm::vec3(0.5), bellow - glm::vec3(0.5) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (above.x < top.x && above.x > bottom.x) {
			if (above.z < top.z && above.z > bottom.z) {
				if (above.y > top.y) {
					return 1;
				}
			}
		}
	}
	else {
		if (above.x == bellow.x && above.z == bellow.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isBellow(glm::vec3 bellow, glm::vec3 above, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { above + glm::vec3(0.5f), above - glm::vec3(0.5f) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (bellow.x < top.x && bellow.x > bottom.x) {
			if (bellow.z < top.z && bellow.z > bottom.z) {
				if (bellow.y < above.y) {
					return 1;
				}
			}
		}
	}
	else {
		if (bellow.x == above.x && bellow.z == above.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isRight(glm::vec3 right, glm::vec3 left, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { left + glm::vec3(0.5f), left - glm::vec3(0.5f) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (right.y < top.y && right.y > bottom.y) {
			if (right.z < top.z && right.z > bottom.z) {
				if (right.x > left.x) {
					return 1;
				}
			}
		}
	}
	else {
		if (right.y == left.y && right.z == left.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isLeft(glm::vec3 left, glm::vec3 right, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { right + glm::vec3(0.5f), right - glm::vec3(0.5f) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (left.y < top.y && left.y > bottom.y) {
			if (left.z < top.z && left.z > bottom.z) {
				if (left.x < right.x) {
					return 1;
				}
			}
		}
	}
	else {
		if (left.y == right.y && left.z == right.z) {
			return 1;
		}
	}
	return 0;
}
GLboolean isInFront(glm::vec3 front, glm::vec3 behind, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { behind + glm::vec3(0.5f), behind - glm::vec3(0.5f) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (front.x < top.x && front.x > bottom.x) {
			if (front.y < top.y && front.y > bottom.y) {
				if (front.z > behind.z) {
					return 1;
				}
			}
		}
	}
	else {
		if (behind.x == front.x && behind.y == front.y) {
			return 1;
		}
	}
	return 0;
}
GLboolean isBehind(glm::vec3 behind, glm::vec3 front, GLboolean isCube = 1) {
	if (isCube) {
		std::pair<glm::vec3, glm::vec3> corners = { front + glm::vec3(0.5f), front - glm::vec3(0.5f) };
		auto& top = corners.first;
		auto& bottom = corners.second;
		if (behind.x < top.x && behind.x > bottom.x) {
			if (behind.y < top.y && behind.y > bottom.y) {
				if (behind.z < front.z) {
					return 1;
				}
			}
		}
	}
	else {
		if (behind.x == front.x && behind.y == front.y) {
			return 1;
		}
	}
	return 0;
}

glm::vec3 Entity::determinCollision(World& world, glm::vec3 deltaV) {
	auto& rayOrigin = pos;
	auto rayEnd = rayOrigin + deltaV;
	Chunk& chunkOccupied = world.getOccupiedChunk(rayEnd);
	auto vertices = getVertices();

	if (chunkOccupied.isNull()) return { 0, 0, 0 };
	glm::vec3 res(0);
	for (auto& mesh : world.getWorldMesh()) {
		Face& face = *mesh;
		glm::vec3 p = std::get<2>(face);
		Buffer* b = std::get<0>(face);
		std::pair<glm::vec3, glm::vec3> corners = { p + glm::vec3(0.5f), p - glm::vec3(0.5f) };
		auto& max = corners.first;
		auto& min = corners.second;
		for (auto vertex : vertices) {
			vertex += rayEnd;
			if (glm::all(glm::lessThanEqual(vertex, max)) && glm::all(glm::greaterThanEqual(vertex, min)) /*|| true*/) {
				switch (b->type)
				{
				case TOP:
					//if (!isAbove(rayEnd, p)) break;
					res.y = -1;
					break;
				case BOTTOM:
					break;
				case LEFT:
					if (!isRight(rayEnd, p)) break;
					res.x = -1;
					break;
				case RIGHT:
					if (!isLeft(rayEnd, p)) break;
					res.x = 1;
					break;
				case FRONT:
					if (!isBehind(rayEnd, p)) break;
					res.z = 1;
					break;
				case BACK:
					if (!isInFront(rayEnd, p)) break;
					res.z = -1;
					break;
				}
			}
			
		}
	}

	return res;
}
glm::vec3 Entity::getCenter() {
	return getCenter(pos);
}
glm::vec3 Entity::getCenter(glm::vec3 pos) {
	return pos;
}
std::vector<glm::vec3> Entity::getVertices() {
	std::vector<glm::vec3> res;
	for (auto& face : body) {
		Buffer* b = std::get<0>(face);
		for (auto& vertex : b->getVertices()) {
			GLboolean found = 0;
			for (auto const& contence : res) {
				if (contence == vertex) {
					found = 1;
					break;
				}
			}
			if (!found) {
				res.push_back(vertex);
			}
		}
	}
	return res;
	
}