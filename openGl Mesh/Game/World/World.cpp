#include "World.h"
World::World() {
	playerPosition = { 0, 0, 0 };
	chunks = std::vector<std::pair<Chunk, GLboolean>>();
}
World::World(GLboolean gen, GLboolean flat) {
	playerPosition = { 0, 0, 0 };
	chunks = std::vector<std::pair<Chunk, GLboolean>>();
	getNewChunkPositions(flat);
}
World::World(Player& player) {
	playerPosition = player.getPosition();
	chunks = std::vector<std::pair<Chunk, GLboolean>>();
}
void World::getNewChunkPositions(GLboolean flat) {
	std::vector<glm::vec3> chunkPositions;
	for (GLint x = -RENDER_DISTANCE / 2; x < RENDER_DISTANCE/2; x++) {
		for (GLint y = 1; y < RENDER_DISTANCE + 1 ; y++) {
			for (GLint z = -RENDER_DISTANCE / 2; z < RENDER_DISTANCE / 2; z++) {
				chunkPositions.push_back({ x * CHUNK_SIZE , y * -CHUNK_SIZE, z * CHUNK_SIZE });
			}
		}
	}
	if (flat) {
		generateFlatChunks(chunkPositions);
	}
	else {
		generateTerrain(chunkPositions);
	}
}
void World::generateFlatChunks(std::vector<glm::vec3> chunkPositions) {
	chunks.resize(RENDERED_VOLUME);
	chunks.reserve(RENDERED_VOLUME);
	std::cout << "Started\n";
	for (auto& pos : chunkPositions) {
		pos -= glm::vec3(0, 0, 0);
		Chunk chunk(pos, false);
		chunk.createBlocks(true);

		glm::vec3& p = chunk.position;

		int index = getChunkIndex(p, false);
		chunks[index] = { chunk, GL_TRUE };
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto& pair : chunks) {
		Chunk& chunk = pair.first;
		chunk.createMesh(getChunks());
		// std::cout << "Chunk created" << std::endl;
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << " size: " << CHUNK_SIZE*std::pow(chunks.size(), 1.0f/3.0f) << "): " << duration.count() << " microseconds" << std::endl;
}
void World::generateTerrain(std::vector<glm::vec3> chunkPositions) {
	chunks.resize(RENDERED_VOLUME);
	chunks.reserve(RENDERED_VOLUME);
	std::cout << "Started\n";
	for (auto& pos : chunkPositions) {
		pos -= glm::vec3(0, 0, 0);
		Chunk chunk(pos, false);
		chunk.createBlocks(false);

		glm::vec3& p = chunk.position;

		int index = getChunkIndex(p, false);
		chunks[index] = { chunk, GL_TRUE };
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto& pair : chunks) {
		Chunk& chunk = pair.first;
		chunk.createMesh(getChunks());
		// std::cout << "Chunk created" << std::endl;
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "World created (Chunk count: " << chunks.size() << " size: " << CHUNK_SIZE * std::pow(chunks.size(), 1.0f / 3.0f) << "): " << duration.count() << " microseconds" << std::endl;
}
void World::renderChunksStatic(Camera c, glm::mat4 projection) {
	drawable.render(c, projection);
}
void World::cleanUp() {
	for (auto& chunk : chunks) {
		chunk.first.cleanUp();
	}
	render.cleanUp();
}
std::vector<Chunk*> World::getChunks() {
	std::vector<Chunk*> res;
	res.reserve(chunks.size());
	for (auto& chunk : chunks) {
		res.push_back(&chunk.first);
	}
	return res;
}
void World::genWorldMesh() {
	std::map<Buffer*, std::vector<Face*>> seperated;
	std::vector<Face*> sorted;
	for (auto& chunk : chunks) {
		for (auto& mesh : chunk.first.getMeshes()) {
			try {
				seperated[std::get<0>(*mesh)].push_back(mesh);
			}
			catch (std::exception e) {
				seperated.insert({ std::get<0>(*mesh), { mesh } });
			}
		}
	}
	for (auto& pair : seperated) {
		sorted.insert(sorted.end(), pair.second.begin(), pair.second.end());
	}
	worldMesh = sorted;
}
void reduceToMultiple(glm::ivec3& victim, GLuint multiple) {
	while (victim.x % multiple != 0) {
		victim.x -= 1;
	}
	while (victim.y % multiple != 0 || victim.y >= 0) {
		victim.y -= 1;
	}
	while (victim.z % multiple != 0) {
		victim.z -= 1;
	}
}
Chunk& World::getOccupiedChunk(glm::ivec3 occPosition) {
	
	reduceToMultiple(occPosition, CHUNK_SIZE);

	int index = getChunkIndex(occPosition, false);
	if (index < 0) {
		Chunk chunk = Chunk();
		return chunk;
	}
	return std::get<0>(chunks[index]);
}