#include "world_generation.h"

glm::ivec2 world_generation::treeCooldown = glm::vec2(4);

GLfloat world_generation::heightAtPositon(glm::vec2 pos, NoiseOptions options, GLuint seed) {

	// Begin iterating through the octaves
	GLfloat value = 0;
	GLfloat accumulatedAmps = 0;
	for (int i = 0; i < options.octaves; i++) {
		GLfloat frequency = glm::pow(2.0f, i);
		GLfloat amplitude = glm::pow(options.roughness, i);

		glm::vec2 xy = (pos * frequency) / options.smoothness;
		xy += seed;

		GLfloat noise = glm::simplex(glm::vec3{ xy.x, xy.y, seed });
		noise = (noise + 1.0f) / 2.0f;
		value += noise * amplitude;
		accumulatedAmps += amplitude;
	}
	return (value) / accumulatedAmps;
}


HeightMap world_generation::createHeightMap(glm::vec2 chunkPos, GLuint seed, GLuint biome) {
	// doesn't generate caves
	NoiseOptions firstNoise;
	firstNoise.amplitude = 105.0f;
	firstNoise.octaves = 6;
	firstNoise.smoothness = 205.0f;
	firstNoise.roughness = 0.58f;
	firstNoise.offset = 18.0f;

	NoiseOptions secondNoise;
	secondNoise.amplitude = 20.0f;
	secondNoise.octaves = 4;
	secondNoise.smoothness = 200.0f;
	secondNoise.roughness = 0.45f;
	secondNoise.offset = 0.0f;

	HeightMap res;

	for (GLubyte x = 0; x < CHUNK_SIZE; x++)
	{
		for (GLubyte y = 0; y < CHUNK_SIZE; y++)
		{
			Timer t;
			t.start();
			std::vector<Block_Count>& encoded = res[x][y];
			glm::vec2 worldPos = { x, y };
			worldPos += chunkPos;
			GLfloat height = heightAtPositon(worldPos, firstNoise, seed);
			GLfloat height2 = heightAtPositon(worldPos, secondNoise, seed);
			GLfloat result = height * height2;
			result *= firstNoise.amplitude + firstNoise.offset - 5;
			result = abs(result);
			if (worldPos == glm::vec2(0)) {
				worldPos.x++;
				height = heightAtPositon(worldPos, firstNoise, seed);
				height2 = heightAtPositon(worldPos, secondNoise, seed);
				result = height * height2;
				result *= firstNoise.amplitude + firstNoise.offset - 5;
				result = abs(result);
			}

			GLfloat result_orig = result;

			if (result < 2) {
				encoded.push_back({ Blocks::WATER, result + 1 });
				break;
			}
			if (result - 3 > 0) {
				encoded.push_back({ Blocks::STONE, result -= 3 });
			}
			if (result_orig <= 20) {
				encoded.push_back({ Blocks::SAND, 3 });
				encoded.push_back({ Blocks::WATER, 20 - result - 2 });
				continue;
			}
			if (result_orig < 23) {
				encoded.push_back({ Blocks::SAND, 3 });
				continue;
			}
			if (result - 2 > 0) {
				encoded.push_back({ Blocks::DIRT, 2 });
			}
			encoded.push_back({ Blocks::GRASS, 1 });
			t.end();
			// t.showTime("hu", 1);
		}
	}
	return res;
}


NewHeightMap world_generation::createHeightMapNew(glm::vec2 chunkPos, unsigned int seed, unsigned int biome) {
	// doesn't generate caves
	NoiseOptions firstNoise;
	firstNoise.amplitude = 105.0f;
	firstNoise.octaves = 6;
	firstNoise.smoothness = 205.0f;
	firstNoise.roughness = 0.58f;
	firstNoise.offset = 18.0f;

	NoiseOptions secondNoise;
	secondNoise.amplitude = 20.0f;
	secondNoise.octaves = 4;
	secondNoise.smoothness = 200.0f;
	secondNoise.roughness = 0.45f;
	secondNoise.offset = 0.0f;

	NewHeightMap res;

	for (unsigned int z = 0; z < CHUNK_SIZE; z++)
	{
		for (unsigned int x = 0; x < CHUNK_SIZE; x++)
		{
			res.emplace_back();
			BlockColumn& encoded = res.back();
			glm::vec2 worldPos = { x, z };
			worldPos += chunkPos; 
			if (worldPos == glm::vec2(0)) worldPos.x++;

			float height1 = heightAtPositon(worldPos, firstNoise, seed);
			float height2 = heightAtPositon(worldPos, secondNoise, seed);
			float height = height1 * height2;
			height *= firstNoise.amplitude + firstNoise.offset - 5;
			height = abs(height);

			float result_orig = height;

			if (height <= 0) {
				encoded.emplace_back((unsigned int) Blocks::WATER, (unsigned int) height);
				break;
			}
			if (height > 3) {
				encoded.emplace_back((unsigned int) Blocks::STONE, (unsigned int) height - 3u);
				encoded.emplace_back((unsigned int) Blocks::DIRT, 2u);
				encoded.emplace_back((unsigned int) Blocks::GRASS, 1u);
			}
		}
	}
	return res;
}

std::vector<glm::vec2> world_generation::getTreePositions(glm::vec2 chunkPos)
{
	std::vector <glm::vec2> trees;
	treeCooldown = { 4, 4 };
	for (GLubyte x = 2; x < CHUNK_SIZE-2; x++)
	{
		for (GLubyte z = 2; z < CHUNK_SIZE-2; z++)
		{
			if (rand() / double(RAND_MAX) < 0.05 && glm::all(glm::lessThanEqual(treeCooldown, glm::ivec2(0)))) {
				// place tree at
				trees.push_back({ x, z });
				treeCooldown = { 4, 4 };
			}
			treeCooldown.y--;
		}
		treeCooldown.x--;
	}
	return trees;
}
