#include "World.h"
#include "gtx/string_cast.hpp"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#pragma region Public
#pragma region Constructors
World::World() {
	chunks = Chunks();
}
World::World(std::string overload) {
	chunks = Chunks();
}
World::World(GLboolean terrain, GLuint seed) {
	this->seed = seed;
	chunks = std::vector<ChunkColumn>();
	// chunkOccupiedPosition = glm::vec2(0);
	std::vector<glm::vec2> positions = centeredPositions({ 0, 0 }, {});
	world_generation wg(seed, 3, 0.75f, { {1.0f, 1.0f}, {2.0f, 2.0f}, {4.0f, 4.0f} });
	AdjacentMap adjacent = getAdjacentMap({ 0, 0, 0 }, RENDER_DISTANCE + 2);
	generateTerrain(positions, adjacent);
}
#pragma endregion

#pragma region getters
std::unordered_map<GLuint, FaceB_p>& World::getWorldMesh()
{
	return worldMesh;
}
#pragma endregion

#pragma region get methods
std::vector<ChunkColumn*> World::getAdjacentChunks(glm::vec3 worldPosition)
{
	std::vector<glm::vec2> chunksToFind;
	std::vector<ChunkColumn*> res;
	res.push_back(getChunkOccupied(worldPosition));
	glm::vec2 centerd = res.back()->getPosition();
	worldPosition.x -= centerd.x;
	worldPosition.z -= centerd.y;
	if (worldPosition.x < 3) chunksToFind.push_back(centerd + glm::vec2(-CHUNK_SIZE, 0));
	else if (worldPosition.x > CHUNK_SIZE - 4) chunksToFind.push_back(centerd + glm::vec2(CHUNK_SIZE, 0));

	if (worldPosition.z < 3) chunksToFind.push_back(centerd + glm::vec2(0, -CHUNK_SIZE));
	else if (worldPosition.z > CHUNK_SIZE - 4) chunksToFind.push_back(centerd + glm::vec2(0, CHUNK_SIZE));

	for (glm::vec2& pos : chunksToFind) {
		std::vector<ChunkColumn>::iterator found = std::find(chunks.begin(), chunks.end(), pos);
		if (found != chunks.end()) {
			res.push_back(&*found);
		}
	}

	return res;
}
AdjacentMap World::getAdjacentMap(glm::vec3 worldPos, GLuint range)
{
	AdjacentMap res;
	std::vector<glm::vec2> poss = centeredPositions({ worldPos.x , worldPos.z }, { }, range);
	for (glm::vec2& pos : poss) {
		Chunks::iterator found = std::find(chunks.begin(), chunks.end(), pos);
		if (found != chunks.end()) {
			res.insert({ pos, *found });
		}
		else {
			HeightMap hm = world_generation::createHeightMap(pos, 0);
			res.insert({ pos, { pos, hm } });
		}
	}
	return res;
}
AdjacentMap_p World::getAdjacentMapPointers(glm::vec3 worldPos, GLuint range)
{
	AdjacentMap_p res;
	std::vector<glm::vec2> poss = centeredPositions({ worldPos.x , worldPos.z }, { }, range);
	for (glm::vec2& pos : poss) {
		Chunks::iterator found = std::find(chunks.begin(), chunks.end(), pos);
		if (found != chunks.end()) {
			res.insert({ pos, &*found });
		}
	}
	return res;
}
ChunkColumn* World::getChunkOccupied(glm::vec3 position) {
	position = glm::floor(position);
	position = reduceToMultiple(position, CHUNK_SIZE);
	glm::vec2 chunkPos(position.x, position.z);
	for (auto& chunk : chunks) {
		if (chunk.getPosition() == chunkPos) {
			return &chunk;
		}
	}
}
#pragma endregion

#pragma region operations
void World::render(Camera& c, glm::mat4 projection) {
	drawable.render(c, projection);
}
void World::updatePlayerPos(glm::vec3 pos) {
	/*glm::vec2 position = getChunkOccupied(pos)->getPosition();
	if (position != chunkOccupiedPosition) {
		chunkOccupiedPosition = position;
	}*/
}
void World::save() {
	return;
	for (auto& chunk : chunks) {
		chunk.save(seed);
	}
}
void World::advanceGeneration() {
	if (generationStack.size() == 0) return;
	AdjacentMap adjacent = getAdjacentMap({ 0, 0, 0 }, RENDER_DISTANCE + 2);
	generationStack.back()->createMesh(adjacent);
	if (generationStack.back()->getStage() == 3) {
		generationStack.pop_back();
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
}
#pragma endregion

#pragma endregion

#pragma region Private

#pragma endregion

void World::generateFlatChunks(std::vector<glm::vec2> chunkPositions) {
	std::cout << "Started\n";
	auto start = std::chrono::high_resolution_clock::now();
	for (glm::vec2& pos : chunkPositions) {
		chunks.push_back({ pos });
	}
	auto t = getAdjacentMap({ 0, 0, 0 }, RENDER_DISTANCE + 2);
	for (auto& chunk : chunks) {
		chunk.createMesh_flat(t);
	}
	genWorldMesh();
	drawable.setUp(worldMesh);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Chunk Creation Duration: " << duration.count() << " microsecconds\n";
}

void World::generateTerrain(std::vector<glm::vec2> chunkPositions, AdjacentMap adjacent) {
	std::cout << "Started\n";

	std::vector<glm::vec2> victims;
	// files and blocks
	for (glm::vec2& pos : chunkPositions) {
		std::string name = "chunk" + std::to_string((int)pos.x) + "," + std::to_string((int)pos.y);
		if (FILE* file = fopen(("Chunks/" + name + ".dat").c_str(), "r")) {
			fclose(file);
			chunks.push_back({ name });
			victims.push_back(pos);
		}
		else {
			HeightMap heightMap = world_generation::createHeightMap(pos, 0);
			chunks.push_back({ pos, heightMap });
		}
	} 
	// adjacent
	adjacent = getAdjacentMap({ 0, 0, 0 }, RENDER_DISTANCE + 2);

	for (auto& victim : victims) {
		auto found = std::find(chunkPositions.begin(), chunkPositions.end(), victim);
		if (found == chunkPositions.end()) continue;
		chunkPositions.erase(found);
	}
	Timer t;
	t.start();
	for (ChunkColumn& chunk : chunks) {
		if (std::find(chunkPositions.begin(), chunkPositions.end(), chunk.getPosition()) != chunkPositions.end()) {
			chunk.createMesh(adjacent); // uses a short cut only looks at the top 6 blocks
			generationStack.push_back(&chunk);
		}
	}
	t.end();
	t.showTime("Creation time");
	genWorldMesh();
	drawable.setUp(worldMesh);

	// th.detach();
	// activeBuffer = activeBuffer1;
}

void World::genWorldMesh() {
	worldMesh.clear();
	for(auto & chunk : chunks) {
		std::unordered_map<GLuint, FaceB_p>& mesh = chunk.getMesh();
		for (auto& m : mesh) {
			const GLuint key = m.first;
			FaceB_p& faces = m.second;
			try {
				FaceB_p& faces2 = worldMesh.at(key);
				std::vector<glm::mat4>& models = std::get<2>(faces);
				std::get<2>(faces2).insert(std::get<2>(faces2).end(), models.begin(), models.end());
			}
			catch (std::exception e) {
				worldMesh.insert({ key, faces });
			}
		}
	}
}

void World::breakBlock(glm::vec3 pos, glm::vec3 front) {
	Ray ray = Ray(pos, front, PLAYER_REACH);
	ChunkColumn* chunkOcc = getChunkOccupied(pos);
	glm::vec2 in_chunkPos = glm::round(glm::vec2(pos.x, pos.z)) - chunkOcc->getPosition();
	
	std::tuple<glm::vec3, FACES_NAMES> intersect = getIntersected(chunkOcc, in_chunkPos, ray);
	glm::vec3 p = std::get<0>(intersect);

	auto t = getAdjacentMapPointers(pos, RENDER_DISTANCE + 2);
	chunkOcc->editBlock(p, 1, Blocks::AIR, t);

	genWorldMesh();
	drawable.setUp(worldMesh);
}
void World::placeBlock(glm::vec3 pos, glm::vec3 front, Blocks block) {
	
	Ray ray = Ray(pos, front, PLAYER_REACH);
	ChunkColumn* chunkOcc = getChunkOccupied(pos);
	glm::vec2 in_chunkPos = glm::round(glm::vec2(pos.x, pos.z)) - chunkOcc->getPosition();

	std::tuple<glm::vec3, FACES_NAMES> intersect = getIntersected(chunkOcc, in_chunkPos, ray);
	auto p = std::get<0>(intersect);
	auto face = std::get<1>(intersect);

	switch (face)
	{
	case FRONT:
		p.z++;
		break;
	case BACK:
		p.z--;
		break;
	case LEFT:
		p.x--;
		break;
	case RIGHT:
		p.x++;
		break;
	case TOP:
		p.y++;
		break;
	case BOTTOM:
		p.y--;
		break;
	case NULL_:
		return;
	}
	// p is the world pos

	auto t = getAdjacentMapPointers(pos);
	chunkOcc->editBlock(p, 1, block, t);

	genWorldMesh();
	drawable.setUp(worldMesh);
}


std::tuple<glm::vec3, FACES_NAMES> World::getIntersected(ChunkColumn*& chunkOcc, glm::vec2 in_chunkPos, Ray ray) {
	std::vector<std::tuple<ChunkColumn*, std::unordered_map<GLuint, FaceB_p>>> meshes;
	for (auto& chunk : chunks) {
		meshes.push_back({ &chunk, chunk.getMesh() });
	}
	

	glm::vec3 p(0);
	FACES_NAMES face = NULL_;
	GLfloat shortestDistance = 1000;
	for (auto& mesh : meshes) {
		std::unordered_map<GLuint, FaceB_p>& faces = std::get<1>(mesh);
		for (auto& faces_ : faces) {
			FACES_NAMES& face_ = std::get<0>(faces_.second)->type;
			for (auto& model : std::get<2>(faces_.second)) {
				glm::vec3 pos = getTranslation(model);
				auto dist = ray.checkIntercesction_Block(pos, face_);
				if (dist == -1) {
					continue;
				}
				if (dist < shortestDistance) {
					shortestDistance = dist;
					p = pos;
					face = face_;
					chunkOcc = std::get<0>(mesh);
				}
			}
		}
	}
	return { p, face };
}

std::vector<ChunkColumn> World::createChunks(std::vector<glm::vec2> positions, std::vector<glm::vec2> activeChunks,
	std::vector<glm::vec2> lazyChunks, GLboolean rectifyForExisting, GLboolean checkLazy, std::vector<ChunkColumn>* adjacentChunks)
{
	return { };
	/*std::vector<ChunkColumn> res;
	for (glm::vec2& chunkPos : positions) {
		HeightMap heightMap = world_generation::createHeightMap(chunkPos, 0);
		res.push_back({ chunkPos, heightMap });
	} // generation of blocks
	
	if (rectifyForExisting) {
		std::vector<glm::vec2> hitList;
		for (ChunkColumn& chunk : chunks2) {
			std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), chunk);
			if (found != res.end()) res.erase(found);
			else {
				hitList.push_back(chunk.getPosition());
			}
		}
		for (glm::vec2& pos : hitList) {
			std::vector<ChunkColumn>::iterator found = std::find(chunks2.begin(), chunks2.end(), pos);
			if (found == chunks2.end()) continue;
			chunks2.erase(found);
		}
	}
	std::vector<ChunkColumn> fromFile;
	if (checkLazy) {
		for (auto& p : std::experimental::filesystem::directory_iterator("Chunks")) {
			if (!p.path().has_extension() || p.path().extension() != ".dat") continue;
			std::string fileName = p.path().filename().string();
			for (auto& l : p.path().extension().string()) {
				fileName.pop_back();
			}
			fileName.erase(fileName.begin(), fileName.begin() + 5);
			std::stringstream ss(fileName);
			std::string token;
			glm::vec2 pos(0);
			GLubyte i = 0;
			while (getline(ss, token, ',')) {
				pos[i++] = std::stof(token);
			}

			std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), pos);
			if (found != res.end()) {
				res.erase(found);
				fromFile.push_back({ "chunk" + fileName });
				// fromFile.back().createMesh({}, 0);
			}
		}
	}
		
	for(glm::vec2 pos : lazyChunks) {
		std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), pos);
		if (found == res.end()) continue;
		ChunkColumn& chunk = *found;
		chunk.createMesh(adjacentChunks);
		chunk.save(seed);
		res.erase(found);
	}
	for (glm::vec2 pos : activeChunks) {
		std::vector<ChunkColumn>::iterator found = std::find(res.begin(), res.end(), pos);
		if (found == res.end()) continue;
		ChunkColumn& chunk = *found;
		chunk.createMesh(adjacentChunks);
	}
	res.insert(res.end(), fromFile.begin(), fromFile.end());
	return res;*/
}

std::vector<glm::vec2> World::centeredPositions(glm::vec2 origin, std::vector<glm::vec2> exclude, GLint renderDist) {
	origin.x = reduceToMultiple(origin.x, CHUNK_SIZE);
	origin.y = reduceToMultiple(origin.y, CHUNK_SIZE);
	if (renderDist % 2 == 0)  renderDist++;
	GLubyte radius = (renderDist - 1) / 2;
	std::vector<glm::vec2> res;
	for (GLbyte x = -radius; x < radius + 1; x++) {
		for (GLbyte y = -radius; y < radius + 1; y++) {
			glm::vec2 pos(x, y);
			pos *= CHUNK_SIZE;
			pos += origin;
			if (std::find(exclude.begin(), exclude.end(), pos) != exclude.end()) continue;
			res.push_back(pos);
		}
	}
	return res;
}
