#pragma once
#include <tuple>
#include <thread>
#include <vector>
#include <chrono>
#include "Chunk.h"
#include "constants.h"
#include "../Player/Player.h"
#include "../../Renders/chunkRender.h"
#include "../../Drawable.h"

class World
{
public:
	World();
	World(Player& player);
	World(GLboolean gen, GLboolean flat = true);
	void renderChunksStatic(Camera c, glm::mat4 projection);
	void renderChunksDynamic();
	void processBlockUpdates();
	void setPlayerPosition(Player& player);
	void cleanUp();
	std::vector<Chunk*> getChunks(); 
	Chunk& getOccupiedChunk(glm::ivec3 occPosition);
private:
	std::vector<std::pair<Chunk, GLboolean>> chunks;
	glm::vec3 playerPosition;
	Render::ChunkMeshRender render;
	std::vector<Face*> worldMesh;
	Drawable drawable;
	void generateFlatChunks(std::vector<glm::vec3> chunkPositions);
	void generateTerrain(std::vector<glm::vec3> chunkPositions);
	void getNewChunkPositions(GLboolean flat);
	void genWorldMesh();
};