#include "Texture.h"

Texture::Texture(std::string name, std::string overload) {
	this->name = name;
	created = 0;
}
Texture::Texture(std::string name, GLboolean is2D) {
	this->name = name;
	this->is2D = is2D;
	if (is2D) {
		created = load2D(name);
	}
	else {
		created = load3D(name);
	}
}
Texture::Texture(GLboolean loadTex) {
	name = "";
	is2D = false;
	created = false;
	if (loadTex) {
		created = load3D("grass");
	}
}
GLboolean Texture::load2D(std::string& name) {
	name = "Textures/" + name + ".png";
	is2D = 1;
	glGenTextures(1, &texMap);
	// diffuse
	unsigned char* image = SOIL_load_image(name.c_str(), &dimentions.x, &dimentions.y, 0, SOIL_LOAD_RGBA);
	if (!image) {
		std::cout << "texture error" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, texMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimentions.x, dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	created = 1;
	return GL_TRUE;
}
GLboolean Texture::load3D(const std::string& name) {
	this->name = name;
	is2D = GL_FALSE;
	std::vector<std::string> faces = {
		"Textures/" + name + "/left.png",	// left
		"Textures/" + name + "/front.png",	// front
		"Textures/" + name + "/bottom.png", // bottom
		"Textures/" + name + "/top.png",	// top
		"Textures/" + name + "/right.png",	// right
		"Textures/" + name + "/back.png"	// back
	};
	glGenTextures(1, &texMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);
	// texMap = texMap;
	unsigned char* data;

	for (GLuint i = 0; i < faces.size(); i++)
	{
		data = SOIL_load_image(faces[i].c_str(), &dimentions.x, &dimentions.y, 0, SOIL_LOAD_RGBA);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, dimentions.x, dimentions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	created = true;
	return GL_TRUE;
}
void Texture::bind() {
	if (!created) return;
	if (is2D) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texMap);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texMap);
	}
}
void Texture::unBind() {
	if (is2D) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
std::string& Texture::getName() {
	return name;
}
GLuint& Texture::getTexMap() {
	return texMap;
}
GLboolean& Texture::get2D() {
	return is2D;
}
glm::ivec2& Texture::getDimentions() {
	return dimentions;
}