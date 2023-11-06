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
		t.buffer->cleanUp();
	}
}

void DrawableGeom::render(Shader* shader) const
{
	if (!shader) {
		shader = &SHADERS[GEOMBLOCKS];
		shader->bind();
	}

	bool b8 = shader->setValue("voxelSize", VOXEL_SZIE);

	draw();

	shader->unBind();
}

void DrawableGeom::setUp(Chunks& chunks)
{
	data.clear();
	for (auto& [pos, chunk] : chunks) {
		data.emplace_back(chunk.getBufferPtr(), nullptr);
	}
}

void DrawableGeom::draw() const
{
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (const DrawData& data : this->data) {
		BufferGeom* buffer = data.buffer;
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
