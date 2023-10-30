#include "World.h"
#include <iostream>
#include "Chunks/ChunkColumn.h"
#include "../../Helpers/Timer.h"

World::World() : chunks(), geomDrawable(), seed(), worldMap() {
}
World::World(bool gen, bool terrain, unsigned int seed) : World() {
	this->seed = seed;
	if (!gen) return;
	worldMap.reserve(RENDER_DISTANCE + 2);
	getNewChunkPositions(!terrain);

}


void World::getNewChunkPositions(bool flat) {
	std::vector<glm::vec2> chunkPositions = centeredPositions(glm::vec2(0), {});

	generateTerrain(chunkPositions);
}

void World::generateTerrain(const std::vector<glm::vec2>& chunkPositions) {
	std::cout << "Started\n";
	// files and blocks
	Timer timer;
	timer.start();
	for (const glm::vec2& pos : chunkPositions) {
		chunks.emplace_back(pos, seed);
	}
	timer.stop();
	unsigned long avg = timer.getTime() / chunkPositions.size();
	std::cout << avg << std::endl;

	for (auto& chunk : chunks) {
		worldMap[chunk.getPosition()] = &chunk.getBlockStore();
	}

	for (auto& chunk : chunks) {
		chunk.populateBuffer(&worldMap);
	}
}

void World::render(Camera& c, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap) const {
	geomDrawable.render(c, projection, lightMatrix, depthMap);
}

void World::setUpDrawable()
{
	geomDrawable.setUp(chunks);
}

const std::vector<glm::vec2> World::centeredPositions(glm::vec2 origin, const std::vector<glm::vec2>& exclude, int renderDist) const {
	origin.x = reduceToMultiple(origin.x, CHUNK_SIZE);
	origin.y = reduceToMultiple(origin.y, CHUNK_SIZE);
	if (renderDist % 2 == 0)  renderDist++;
	char radius = (renderDist - 1) / 2;
	std::vector<glm::vec2> res;
	for (char x = -radius; x < radius + 1; x++) {
		for (char y = -radius; y < radius + 1; y++) {
			glm::vec2 pos(x, y);
			pos *= CHUNK_SIZE;
			pos += origin;
			if (exclude.size() > 0 && std::find(exclude.begin(), exclude.end(), pos) != exclude.end()) continue;
			res.push_back(pos);
		}
	}
	return res;
}
