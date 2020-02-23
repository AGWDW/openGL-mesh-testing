#include "chunk_column.h"
chunk_column::chunk_column() :pos(0), hasCaves(0){
	genHeightMap(0);
}
chunk_column::chunk_column(GLboolean gen) : pos(0), hasCaves(0) {
	if(gen) genHeightMap(0);
}
chunk_column::chunk_column(ChunkPosition pos, GLboolean isFlat) : hasCaves(0) {
	this->pos = pos;
	createChunks();
	genHeightMap(isFlat);
}
chunk_column::chunk_column(ChunkPosition pos, HeightMap hm) : hasCaves(0) {
	this->pos = pos;
	this->heightMap = hm;
	applyHeightMap();
}
void chunk_column::genHeightMap(GLboolean isFlat) {
	if (isFlat) {
		for (auto& chunk : chunks) {
			chunk.createBlocks(1, 1);
		}
	}
	else {
		heightMap = world_generation::createHeightMap({ pos.x, 0, pos.y }, 0);
	}
}
std::vector<Chunk*> getChunks(std::vector<Chunk>& chunks) {
	std::vector<Chunk*> res;
	for (auto& chunk : chunks) {
		res.push_back(&chunk);
	}
	return res;
}
void chunk_column::createMesh(std::vector<chunk_column*> columns) {
	std::vector<Chunk*> chunks;
	for (auto& column : columns) {
		std::vector<Chunk*> subChunks = getChunks(column->chunks);
		chunks.insert(chunks.end(), subChunks.begin(), subChunks.end());
	}
	for (auto& chunk : this->chunks) {
		chunk.createMesh(chunks);
		auto t = chunk.getMeshes();
		faces.insert(faces.end(), t.begin(), t.end());
	}
}
glm::vec3 chunk_column::fromIndex(GLushort index) {
	glm::vec3 pos(this->pos.x, 0, this->pos.y);
	pos.y = -(index + 1) * CHUNK_SIZE;
	return pos;
}
void chunk_column::applyHeightMap() {
	if (hasCaves) {
		// deal with caves
	}
	else {
		auto range = getHeightRange();
		auto& max = range.first;
		auto& min = range.second;
		auto i_raw = reduceToMultiple(min, CHUNK_SIZE);
		auto i = i_raw / CHUNK_SIZE;
		for (GLubyte j = 0; j < i; j++) {
			chunks[j].createBlocks(1, 5);
		}
		ChunkHeightMap hm;
		for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
			for (GLubyte z = 0; z < CHUNK_SIZE; z++) {
				auto newHeight = heightMap[x][z].size() - i_raw;
				if (newHeight >= CHUNK_SIZE) newHeight = CHUNK_SIZE - 1;
				hm[x][z] = newHeight;
			}
		}
		if (max - CHUNK_SIZE >= CHUNK_SIZE) {
			chunks.push_back(Chunk(fromIndex(chunks.size()), 0));
		}

		for (GLubyte x = 0; x < CHUNK_SIZE; x++) {
			for (GLubyte z = 0; z < CHUNK_SIZE; z++) {

				hm[x][z] = heightMap[x][z].size() - hm[x][z];
			}
		}
		chunks.back().createBlocks(hm);
	}
}
void chunk_column::addChunk(Chunk chunk, GLboolean sort) {
	chunks.push_back(chunk);
	if (sort) sortChunks();
}
void chunk_column::addChunks(std::vector<Chunk> chunks, GLboolean sort) {
	this->chunks.insert(this->chunks.end(), chunks.begin(), chunks.end());
	if (sort) sortChunks();
}
void chunk_column::sortChunks() {
	std::vector<std::pair<Chunk*, GLubyte>> chunkIndices;
	for (auto& chunk : chunks) {
		chunkIndices.push_back({ &chunk, std::abs(chunk.position.y / CHUNK_SIZE + 1) });
	}
	std::vector<Chunk> sorted;
	sorted.resize(chunks.size());
	for (auto& pair : chunkIndices) {
		sorted[pair.second] = *pair.first;
	}
	chunks = sorted;
}
std::pair<GLushort, GLushort> chunk_column::getHeightRange() {
	GLushort largest = 0, smallest = 0;
	for (GLushort x = 0; x < CHUNK_SIZE; x++) {
		for (GLushort z = 0; z < CHUNK_SIZE; z++) {
			if (hasCaves) {
				// deal with caves
			}
			else {
				Height height = heightMap[x][z].size();
				if (height > largest) largest = height;
				else if (height < smallest) smallest = height;
			}
		}
	}
	return { largest, smallest };
}
Faces chunk_column::getMesh() {
	return faces;
}void chunk_column::createChunks() {
	chunks.clear();
	GLubyte number = 64 / CHUNK_SIZE;
	for (GLubyte i = 0; i < number; i++) {
		chunks.push_back(Chunk({ pos.x, -(i + 1) * CHUNK_SIZE, pos.y }, 0));
	}
}