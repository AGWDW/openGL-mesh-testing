#pragma once
#include <tuple>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <gtc/noise.hpp>

#include "constants.h"
#include "../../Drawable.h"
#include "../../Ray.h"
#include "Chunks/ChunkColumn.h"

class World
{
private: // properties
	Chunks chunks;
	GLuint seed;
	Drawable drawable;
	std::unordered_map<GLuint, FaceB_p> worldMesh;
	// glm::vec2 chunkOccupiedPosition;
	std::vector<ChunkColumn*> generationStack;	

public: // functions
	// constructors
	World();
	World(std::string overload);
	World(GLboolean terrain = 1, GLuint seed = CHUNK_SIZE*2);

	// getters
	std::unordered_map<GLuint, FaceB_p>& getWorldMesh();
	// get methods
	std::vector<ChunkColumn*> getAdjacentChunks(glm::vec3 worldPosition);
	AdjacentMap getAdjacentMap(glm::vec3 worldPos, GLuint range = RENDER_DISTANCE);
	AdjacentMap_p getAdjacentMapPointers(glm::vec3 worldPos, GLuint range = RENDER_DISTANCE);
	ChunkColumn* getChunkOccupied(glm::vec3 position);

	// editors
	void breakBlock(glm::vec3 pos, glm::vec3 front);
	void placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block);


	// operations
	void render(Camera& c, glm::mat4 projection);
	void updatePlayerPos(glm::vec3 pos);
	void save();
	void advanceGeneration();
private: // functions
	std::vector<glm::vec2> centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, GLint renderDist = RENDER_DISTANCE);


	void getNewChunkPositions(GLboolean flat);
	std::vector<glm::vec2> getNewChunkPositions(glm::vec2 origin, GLint renderDist = INITALL_VIEW);

	void generateFlatChunks(std::vector<glm::vec2> chunkPositions);
	void generateTerrain(std::vector<glm::vec2> chunkPositions, AdjacentMap adjacent);

	void genWorldMesh();

	std::tuple<glm::vec3, FACES_NAMES> getIntersected(ChunkColumn*& chunkOcc, glm::vec2 in_chunkPos, Ray ray);

	std::vector<ChunkColumn> createChunks(std::vector<glm::vec2> positions, std::vector<glm::vec2> activeChunks,
		std::vector<glm::vec2> lazyChunks, GLboolean rectifyForExisting, GLboolean checkLazy, std::vector<ChunkColumn>* subChunks);
};