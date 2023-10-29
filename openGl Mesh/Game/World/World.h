#pragma once
#include <vector>

#include "constants.h"
#include "../../GeomRendering/DrawableGeom.h"

class World
{
public:
	World();
	World(bool gen, bool terrain = true, unsigned int seed = SEED);

	void render(Camera& c, glm::mat4 projection, glm::mat4 lightMatrix, unsigned int depthMap) const;

	void setUpDrawable();

private:
	unsigned int seed;
	std::vector<ChunkColumn> chunks;

	DrawableGeom geomDrawable;

	WorldMap worldMap;

	const std::vector<glm::vec2> centeredPositions(glm::vec2 origin, const std::vector<glm::vec2>& exclude, int renderDist = RENDER_DISTANCE) const;
	void getNewChunkPositions(bool flat);
	void generateTerrain(const std::vector<glm::vec2>& chunkPositions);
};