#pragma once
#include <glm.hpp>

class BoxCollider
{
public:
	BoxCollider();
	BoxCollider(glm::vec3 dimentions, glm::vec3 position);
	bool isColliding(BoxCollider& collider);
	void setPosition(glm::vec3 position);
	void setDimentions(glm::vec3 dimMin, glm::vec3 dimMax);
protected:
	glm::vec3 dimentions;
	glm::vec3 min, max, dimMin, dimMax;
	glm::vec3 position;
private:
	void updateMinMax();
};

