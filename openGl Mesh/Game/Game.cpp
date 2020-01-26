#include "Game.h"

#pragma region GameConfig
GLboolean GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.5, 0.5, 0.5 };
GLuint GameConfig::FPSlock = 0;
#pragma endregion

#pragma region Static Members
Camera* Game::mainCamera = new Camera({ 0, 2, 0 });
glm::vec3 Game::mouseData(0);
std::array<GLboolean, 1024> Game::keys = std::array<GLboolean, 1024>();
#pragma endregion
Game::Game() {
	hasPlayer = false;
	gameRunning = false;
	Game::mainCamera = new Camera({ 0, 2, 0 });
	GameConfig::setup();
}
Game::Game(GLboolean hasPlayer) {
	this -> hasPlayer = hasPlayer;

	gameRunning = false;
	GameConfig::setup();
}
void Game::generateWorld() {
	world = World(true);
}
void Game::update() {

}
void Game::doLoop(glm::mat4 projection) {
	gameRunning = true;
	setupEventCB(window);
	this->projection = projection;
	Mesh::FaceMesh top(FACES[TOP], TEXTURES[GRASS]);
	top.setPosition({ 0, 2, 0 });
	Render::ChunkMeshRender rend;
	// rend.loadMeshes({ top });
	while (gameRunning) {
		calcTimes();
		lockFPS();
		showFPS();
		proccesEvents();
		doMovement();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		showStuff();
		// rend.render(*Game::mainCamera, projection);


		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
	}
}
void Game::calcTimes() {
	GLfloat frame = glfwGetTime();
	deltaTime = frame - lastFrameTime;
	lastFrameTime = frame;
	frameRate = 1 / deltaTime;
}
void Game::showFPS() {
	if (GameConfig::showFPS) {
		std::cout << "FPS: " << frameRate << std::endl;
	}
}
void Game::proccesEvents() {
	glfwPollEvents();
}
void Game::lockFPS() {
	if (GameConfig::FPSlock > 0) {
		if (frameRate > GameConfig::FPSlock) {
			GLfloat toEllaps = (GLfloat)1 / (frameRate - GameConfig::FPSlock);
			while (toEllaps < 0) toEllaps *= 10;
			std::this_thread::sleep_for(std::chrono::milliseconds((GLuint)toEllaps));
		}
	}
}
void Game::showStuff(GLboolean showStatic) {
	if (hasPlayer) {
		// player.render(projection);
	}
	if (showStatic) {
		world.renderChunksStatic(*mainCamera, projection);
	}
	else {
		// world.renderChunksDynamic();
	}
}
void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}

void Game::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS || action == GLFW_RELEASE) {
			Game::keys[key] ^= true;
		}
	}
}
void Game::mouseCallBack(GLFWwindow* window, double xPos, double yPos) {
	if ((int)Game::mouseData.z) {
		Game::mouseData.x = xPos;
		Game::mouseData.y = yPos;
		Game::mouseData.z = false;
	}

	GLfloat xOffset = xPos - Game::mouseData.x;
	GLfloat yOffset = Game::mouseData.y - yPos;

	Game::mouseData.x = xPos;
	Game::mouseData.y = yPos;

	Game::mainCamera->ProcessMouseMovement(xOffset, yOffset);
}
void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
}
void Game::doMovement() {
	if (Game::keys[GLFW_KEY_W] || Game::keys[GLFW_KEY_UP]) {
		Game::mainCamera->ProcessMovement(FORWARD, deltaTime);
		/*Physics::Update up = p.processMovement(FORWARD, deltaTime);
		Physics::Engine::addUpdate(up); */
	}
	if (Game::keys[GLFW_KEY_S] || Game::keys[GLFW_KEY_DOWN]) {
		Game::mainCamera->ProcessMovement(BACKWARD, deltaTime);
		/*Physics::Update up = p.processMovement(BACKWARD, deltaTime);
		Physics::Engine::addUpdate(up);*/
	}
	if (Game::keys[GLFW_KEY_D] || Game::keys[GLFW_KEY_RIGHT]) {
		Game::mainCamera->ProcessMovement(RIGHT_C, deltaTime);
		/*Physics::Update up = p.processMovement(RIGHT, deltaTime);
		Physics::Engine::addUpdate(up);*/
	}
	if (Game::keys[GLFW_KEY_A] || Game::keys[GLFW_KEY_LEFT]) {
		Game::mainCamera->ProcessMovement(LEFT_C, deltaTime);
		/*Physics::Update up = p.processMovement(LEFT, deltaTime);
		Physics::Engine::addUpdate(up);*/
	}
}
void Game::cleanUp() {
	world.cleanUp();
}


