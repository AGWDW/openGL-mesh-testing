#include "DrawableGeom.h"
#include "DrawData.h"
#include "BufferGeom.h"
#include "../Game/Player/Camera.h"
#include "../Game/World/Chunks/ChunkColumn.h"
#include "../Shaders/Shader.h"
#include "../Textures/Texture.h"

DrawableGeom::DrawableGeom() : data()
{
}

DrawableGeom::~DrawableGeom()
{
	for (auto& t : data) {
		t.buffer.cleanUp();
	}
}

void DrawableGeom::render(Shader* shader) const
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	if (!shader) {
		shader = &SHADERS[GEOMBLOCKS];
		shader->bind();
	}

	bool b8 = shader->setValue("voxelSize", VOXEL_SZIE);

	draw();

	shader->unBind();
	glDisable(GL_CULL_FACE);
}

void DrawableGeom::setUp(const Chunks& chunks)
{
	data.clear();
	for (auto& chunk : chunks) {
		data.emplace_back(chunk.getBuffer(), nullptr);
	}
}

void DrawableGeom::draw() const
{
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (const DrawData& data : this->data) {
		const BufferGeom* buffer = &data.buffer;
		Texture* tex = data.texture;
		if (tex) {
			tex->bind();
		}

		buffer->bind();
		glDrawArrays(GL_POINTS, 0, buffer->size());
		buffer->unbind();
		if (tex) tex->unBind();
	}
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
