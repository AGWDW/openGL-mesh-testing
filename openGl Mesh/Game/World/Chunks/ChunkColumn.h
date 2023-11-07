#pragma once
#include "../../../BlockStore.h"
#include "../../../GeomRendering/BufferGeom.h"
#include "../../../GeomRendering/GeomData.h"

class World;

class ChunkColumn
{
public:
	ChunkColumn();
	ChunkColumn(glm::vec2 pos, unsigned int seed, WorldMap& map);

	// creators
	// will build this chunk inplace
	void build(glm::vec2 pos, unsigned int seed, const std::vector<ChunkColumn*>& neibours);
	void populateBuffer(const std::vector<ChunkColumn*>& neibours, const BlockStore& blockStore);
	void populateBuffer(WorldMap& worldMap);
	const BufferGeom& getBuffer() const;
	BufferGeom* getBufferPtr();

	void addBlock(const glm::vec3& worldPos, const Block block);
	void removeBlock(const glm::vec3& worldPos, World* world);

	const glm::vec2& getPosition() const;

	void save() const;
	void load(const glm::vec2& chunkPos);
private:
	struct AddFaces {
		glm::vec3 worldPos;
		glm::vec3 offset;
		unsigned char face;
	};
	std::unordered_map<glm::vec3, Block> editedBlocks;
	std::vector<GeomData> bufferData;
	BufferGeom buffer;
	glm::vec2 position;

	unsigned int seed;

	const Block getBlock(glm::vec3 pos, bool worldPos, bool safe, WorldMap& worldMap) const;
	/// <summary>
	/// Gets the block from edited or simplex noise
	/// </summary>
	/// <param name="relativePos">ranges from (0-15)</param>
	/// <returns>thhe found block</returns>
	const Block getBlock(const glm::vec3& worldPos);
	const Block getBlock(const glm::vec3& worldPos, const std::vector<ChunkColumn*>& neibours, const BlockStore& bs);

	const Block getBlock(glm::vec3 pos, bool worldPos, const BlockStore& blockStore) const;
	const glm::vec3 getRelativePosition(glm::vec3 worldPos) const;
	const glm::vec3 getWorldPosition(glm::vec3 relativePos) const;

	void addFace(const AddFaces& data);

	static bool outOfRange(const glm::vec3& worldPos);

	glm::vec3 getWorldPos() const;
	glm::vec3 toLocal(const glm::vec3& p) const;
};