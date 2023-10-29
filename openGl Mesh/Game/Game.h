#pragma once
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <array>
#include <chrono>
#include <thread>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <time.h>

#include "../Renders/UI Stuff/UI_Renderer.h"
#include "../EntityHander.h"
#include "Player/Entity.h"
#include "World/World.h"
#include "../Ray.h"
struct Character {
	unsigned int     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	unsigned int     Advance;    // Offset to advance to next glyph
};
struct GameConfig {
	static bool showFPS;
	static glm::vec3 backgroundCol;
	static unsigned int FPSlock;
	static void setup(bool showFPS = false, glm::vec3 bgCol = { 0.5, 0.5, 0.5 }, unsigned int FPS = 0) {
		GameConfig::showFPS = showFPS;
		GameConfig::backgroundCol = bgCol;
		GameConfig::FPSlock = FPS;
	}
};
class Game
{
public:
	Game();
	Game(bool hasPlayer, bool hasSkybox, glm::ivec2 windowDim);
	void generateWorld();
	void setupPlayer();
	void doLoop(glm::mat4 projection);

	void setWindow(GLFWwindow* window);
	void cleanUp();

	static Camera* mainCamera;
	static glm::vec3 mouseData;
	static std::array<bool, 1024> keys;
	Ray ray;
private:
	GLFWwindow* window;
	float deltaTime;
	unsigned int frameRate;
	bool gameRunning;
	static World world;
	static Entity* player;
	static EntityHander entityHander;
	static UI_Renderer uiRenderer;
	bool hasSkybox;
	float lastFrameTime;
	glm::mat4 projection, lightProjection;
	unsigned int SBVAO,LSVAO, LSVBO, depthFBO;
	std::map<GLchar, Character> Letters;
	unsigned int depthMap;
	glm::ivec2 windowDim;

	void showFPS();
	void calcTimes();
	void proccesEvents();
	void setupEventCB(GLFWwindow* window);
	void showStuff();
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouseCallBack(GLFWwindow* window, double xPos, double yPos);
	static void clickCallBack(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
	void processKeys();
	void makeSkybox(std::string skybox);
	void showSkybox();
	void setUpFreeType();
	void showText(std::string text, glm::vec2 position, float scale = 1.0f, glm::vec3 colour = glm::vec3(1));
	void createGUI();
	void showGUI();

	void setupDepthFBO();
};