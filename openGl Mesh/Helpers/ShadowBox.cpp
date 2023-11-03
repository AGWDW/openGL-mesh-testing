#include "ShadowBox.h"
#include "Functions.h"
#include "../Game/Player/Camera.h"
#include <gtx/string_cast.hpp>


ShadowBox::ShadowBox() : lightViewMatrix(), OFFSET(10), minX(), minY(), minZ(), maxX(), maxY(), maxZ(), farWidth(), farHeight(), nearWidth(), nearHeight() { }

ShadowBox::ShadowBox(glm::vec3 a) : ShadowBox() {
	this->lightViewMatrix = glm::translate(glm::mat4(1), a);
	calculateWidthsAndHeights();
}

void ShadowBox::update(Camera& camera) {
	glm::vec3 forwardVector = camera.GetFront();

	glm::vec3 toFar = forwardVector * SHADOW_DISTANCE;
	glm::vec3 toNear = forwardVector * NEAR_PLANE;
	glm::vec3 centerNear = toNear + camera.GetPosition();
	glm::vec3 centerFar = toFar + camera.GetPosition();

	std::array<glm::vec4, 8> points = calculateFrustumVertices(forwardVector, centerNear, centerFar);

	bool first = true;
	for (const glm::vec4& point : points) {
		if (first) {
			minX = point.x;
			maxX = point.x;
			minY = point.y;
			maxY = point.y;
			minZ = point.z;
			maxZ = point.z;
			first = false;
			continue;
		}
		if (point.x > maxX) {
			maxX = point.x;
		}
		else if (point.x < minX) {
			minX = point.x;
		}
		if (point.y > maxY) {
			maxY = point.y;
		}
		else if (point.y < minY) {
			minY = point.y;
		}
		if (point.z > maxZ) {
			maxZ = point.z;
		}
		else if (point.z < minZ) {
			minZ = point.z;
		}
	}
	maxZ += OFFSET;

}

const glm::vec3 ShadowBox::getCenter()  const {
	float x = (minX + maxX) * 0.5f;
	float y = (minY + maxY) * 0.5f;
	float z = (minZ + maxZ) * 0.5f;
	glm::vec4 cen(x, y, z, 1);
	glm::mat4 invertedLight = glm::inverse(lightViewMatrix);
	return glm::vec3(invertedLight * cen);
}

const float ShadowBox::getWidth()  const {
	return maxX - minX;
}


const float ShadowBox::getHeight()  const {
	return maxY - minY;
}

const float ShadowBox::getLength()  const {
	return maxZ - minZ;
}

std::array<glm::vec4, 8> ShadowBox::calculateFrustumVertices(glm::vec3 forwardVector, glm::vec3 centerNear, glm::vec3 centerFar) {
	glm::vec3 upVector(0, 1, 0);
	glm::vec3 rightVector = glm::cross(forwardVector, upVector);
	glm::vec3 downVector = -upVector;
	glm::vec3 leftVector = -rightVector;

	glm::vec3 farTop = centerFar + upVector * farHeight;
	glm::vec3 farBottom = centerFar + downVector * farHeight;

	glm::vec3 nearTop = centerNear + upVector * nearHeight;
	glm::vec3 nearBottom = centerNear + downVector * nearHeight;

	std::array<glm::vec4, 8> points{};
	points[0] = calculateLightSpaceFrustumCorner(farTop, rightVector, farWidth);
	points[1] = calculateLightSpaceFrustumCorner(farTop, leftVector, farWidth);
	points[2] = calculateLightSpaceFrustumCorner(farBottom, rightVector, farWidth);
	points[3] = calculateLightSpaceFrustumCorner(farBottom, leftVector, farWidth);
	points[4] = calculateLightSpaceFrustumCorner(nearTop, rightVector, nearWidth);
	points[5] = calculateLightSpaceFrustumCorner(nearTop, leftVector, nearWidth);
	points[6] = calculateLightSpaceFrustumCorner(nearBottom, rightVector, nearWidth);
	points[7] = calculateLightSpaceFrustumCorner(nearBottom, leftVector, nearWidth);
	return points;
}

glm::vec4 ShadowBox::calculateLightSpaceFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width) {
	glm::vec3 point = startPoint + direction * width;
	glm::vec4 point4f = glm::vec4(point.x, point.y, point.z, 1);
	return lightViewMatrix * point4f;
}

void ShadowBox::calculateWidthsAndHeights() {
	farWidth = SHADOW_DISTANCE * tanf(radians(FOV));
	nearWidth = NEAR_PLANE * tanf(radians(FOV));
	farHeight = farWidth / ASPECT_RATIO;
	nearHeight = nearWidth / ASPECT_RATIO;
}

glm::mat4 ShadowBox::getProjection() const
{
	return glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
}
glm::mat4 ShadowBox::getView(const glm::vec3& lightPos)
{
	glm::vec3 dir = glm::normalize(-lightPos);
	glm::mat4 res(1);
	float pitch = acosf(glm::length(glm::vec2(dir.x, dir.z)));
	res = glm::rotate(res, pitch, glm::vec3(1, 0, 0));
	float yaw = atanf(dir.x / dir.z);
	yaw = dir.z > 0 ? yaw - PI : yaw;
	res = glm::rotate(res, -yaw, glm::vec3(0, 1, 0));

	res = glm::translate(res, getCenter());
	// lightViewMatrix = res;

	glm::vec3 nearCenter(getWidth() * .5f, getHeight() * .5f, minZ); // in light space
	glm::vec3 farCenter(nearCenter.x, nearCenter.y, maxZ);
	glm::vec3 up(0, 1, 0);
	glm::vec3 fwd = dir;
	glm::vec3 right = glm::normalize(glm::cross(fwd, up));

	// glm::mat4 res1 = glm::mat4(1);
	// res1[0][0] = right.x;
	// res1[0][1] = right.y;
	// res1[0][2] = right.z;
	// 
	// res1[1][0] = up.x;
	// res1[1][1] = up.y;
	// res1[1][2] = up.z;
	// 
	// res1[2][0] = fwd.x;
	// res1[2][1] = fwd.y;
	// res1[2][2] = fwd.z;
	// 
	// res1[3][0] = nearCenter.x;
	// res1[3][1] = nearCenter.y;
	// res1[3][2] = nearCenter.z;
	// res = glm::lookAt(nearCenter, getCenter(), glm::vec3(0, 1, 0));
	return res;
}

std::vector<glm::vec3> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
	std::vector<glm::vec3> frustrumCorners = {
		glm::vec3(-1,  1, -1),
		glm::vec3( 1,  1, -1), // maybe z = 0
		glm::vec3( 1, -1, -1),
		glm::vec3(-1, -1, -1),

		glm::vec3(-1,  1, 1),
		glm::vec3( 1,  1, 1),
		glm::vec3( 1, -1, 1),
		glm::vec3(-1, -1, 1),
	};

	glm::mat4 pvInv = glm::inverse(proj * view);
	for (glm::vec3& c : frustrumCorners) {
		auto t = pvInv * glm::vec4(c, 1);
		c = glm::vec3(t) / t.w;
	}
	return frustrumCorners;
}

glm::mat4 ShadowBox::getLSM(Camera& camera, const glm::mat4& proj, const glm::vec3& lightPos) const
{
	auto corners = getFrustumCornersWorldSpace(proj, camera.GetViewMatrix());
	glm::vec3 frustrumCenter(0);
	for (const glm::vec3& c : corners) {
		frustrumCenter += c;
	}
	frustrumCenter /= 8.f;

	float radius = glm::distance(corners[3], corners[5]) * .5f;
	float texelsPerUnit = SHADOW_MAP_SIZE / (radius * 2.f);

	glm::mat4 scalar(texelsPerUnit);
	scalar[3][3] = 1;
	glm::vec3 lightDir = glm::normalize(-lightPos);
	glm::mat4 lookat = glm::lookAt(glm::vec3(0), lightDir, glm::vec3(0, 1, 0));
	lookat *= scalar;
	glm::mat4 lookatInv = glm::inverse(lookat);

	glm::vec4 t = lookat * glm::vec4(frustrumCenter, 1);
	frustrumCenter = glm::vec3(t) / t.w;
	frustrumCenter.x = floorf(frustrumCenter.x);
	frustrumCenter.z = floorf(frustrumCenter.z);
	t = lookatInv * glm::vec4(frustrumCenter, 1);
	frustrumCenter = glm::vec3(t) / t.w;

	glm::vec3 eye = frustrumCenter - (lightDir * radius * 2.f);

	glm::mat4 lightView = glm::lookAt(eye, frustrumCenter, glm::vec3(0, 1, 0));

	glm::mat4 lightProj = glm::ortho(-radius, radius, -radius, radius, NEAR_PLANE, 12.f * radius);

	return lightProj * lightView;
}

