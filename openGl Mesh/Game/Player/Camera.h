#pragma once

#include <vector>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include "../World/constants.h"



const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 6.0f;
const float SENSITIVITY = 0.25f;
const float ZOOM = 45.0f; // the field of view

class Camera {
public:
	Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0), float yaw = YAW,
		float pitch = PITCH) : front(glm::vec3(0, 0, -1)), movementSpeed(SPEED), mouseSensitivaty(SENSITIVITY), zoom(ZOOM) {
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
		float pitch) : front(glm::vec3(0, 0, -1)), movementSpeed(SPEED), mouseSensitivaty(SENSITIVITY), zoom(ZOOM) {
		this->position = glm::vec3(posX, posY, posZ);
		this->worldUp = glm::vec3(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}
	glm::vec3 GetRight() {
		return right;
	}

	void ProcessMovement(Move_Dir direction, float deltaTime) {
		float velocity = this->movementSpeed * deltaTime;
		/*if (direction == Move_Dir::FORWARD) {
			this->position += this->front * velocity;
		}
		if (direction == Move_Dir::BACKWARD) {
			this->position -= this->front * velocity;
		}
		if (direction == Move_Dir::LEFT) {
			this->position -= this->right * velocity;
		}
		if (direction == Move_Dir::RIGHT) {
			this->position += this->right * velocity;
		}*/
		/*switch (direction)
		{
		case Move_Dir::FORWARD:
			this->position += this->front * velocity;
		case Move_Dir::BACKWARD:
			this->position -= this->front * velocity;
		case Move_Dir::LEFT:
			this->position -= this->right * velocity;
		case Move_Dir::RIGHT:
			this->position += this->right * velocity;
		case Move_Dir::UP:
			this->position += glm::vec3(0, 1, 0) * velocity;
		case Move_Dir::DOWN:
			this->position -= glm::vec3(0, 1, 0) * velocity;
		}*/
		switch (direction)
		{
		case Move_Dir::FORWARD:
			this->position += glm::vec3(0, 0, -1) * velocity;
		case Move_Dir::BACKWARD:
			this->position += glm::vec3(0, 0, 1) * velocity;
		case Move_Dir::LEFT:
			this->position += glm::vec3(-1, 0, 0) * velocity;
		case Move_Dir::RIGHT:
			this->position += glm::vec3(1, 0, 0) * velocity;
		case Move_Dir::UP:
			this->position += glm::vec3(0, 1, 0) * velocity;
		case Move_Dir::DOWN:
			this->position += glm::vec3(0, -1, 0) * velocity;
		}
	}
	void t() {
		std::cout << this->position.x << std::endl;
	}
	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true) {
		xOffset *= this->mouseSensitivaty;
		yOffset *= this->mouseSensitivaty;

		this->yaw += xOffset;
		this->pitch += yOffset;// up down

		if (constrainPitch) {
			if (this->pitch > 89.0f) {
				this->pitch = 89.0f;
			}
			else if (this->pitch < -89.0f) {
				this->pitch = -89.0f;
			}
		}
		// this->position = glm::vec3(0);

		this->updateCameraVectors();
	}
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		front.y = sin(glm::radians(this->pitch));
		front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}
	float GetZoom() {
		return this->zoom;
	}
	glm::vec3& GetPosition() {
		return this->position;
	}
	glm::vec3 GetFront() {
		return this->front;
	}
	void setPosition(glm::vec3 position) {
		this->position = position;
	}
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 worldUp;
	float mouseSensitivaty;
	float pitch;
	float yaw;
	float movementSpeed;
	float zoom;
	
	glm::vec3 tr;
};