#include "Drawable.h"
Drawable::Drawable() {

}
Drawable::Drawable(std::vector<Mesh::FaceMesh*>& sortedMeshes) {
	setUp(sortedMeshes);
}
void Drawable::setUp(std::vector<Mesh::FaceMesh*>& sortedMeshes) {
	Buffer* prevBuffer = sortedMeshes[0]->buffer;
	Texture* prevTex = sortedMeshes[0]->texture;;
	GLuint counter = 0;
	std::vector<glm::mat4> positions;

	for (auto& mesh : sortedMeshes) {
		if (mesh->buffer != prevBuffer || mesh->texture != prevTex) {
			prevBuffer->addPositions(positions);
			buffers.push_back({ *prevBuffer, prevTex, counter });
			prevBuffer->resetData();
			// reset
			counter = 0;
			positions.clear();
			prevBuffer = mesh->buffer;
			prevTex = mesh->texture;
			prevBuffer->resetData();
		}
		glm::mat4 model(1);
		model = glm::translate(model, mesh->position);
		positions.push_back(model);
		counter += 1;
	}
	prevBuffer->addPositions(positions);
	buffers.push_back({ *prevBuffer, prevTex, counter });
}
void Drawable::render(Camera& cam, glm::mat4 projection) {
	Shader* shader = SHADERS[BLOCK3];
	shader->bind();

	glm::mat4 view(1);
	view = cam.GetViewMatrix();
	shader->setValue("view", view);
	shader->setValue("projection", projection);

	glm::vec3 viewPos = cam.GetPosition();
	shader->setValue("viewPos", viewPos);

	for (auto& buffer : buffers) {
		Buffer& b = std::get<0>(buffer);
		Texture* t = std::get<1>(buffer);
		GLuint c = std::get<2>(buffer);
		t->bind();
		b.render(false);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, c);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	shader->unBind();
}