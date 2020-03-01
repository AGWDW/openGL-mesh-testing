#include "Game.h"

#pragma region GameConfig
GLboolean GameConfig::showFPS = false;
glm::vec3 GameConfig::backgroundCol = { 0.0f, 0.0f, 0.0f };
GLuint GameConfig::FPSlock = 0;
#pragma endregion

#pragma region Static Members
Camera* Game::mainCamera = new Camera({ 0, 2, 0 });
glm::vec3 Game::mouseData(0);
std::array<GLboolean, 1024> Game::keys = std::array<GLboolean, 1024>();
Player Game::player = Player();
GLboolean Game::hasPlayer = GL_FALSE;
World Game::world = World(0);
#pragma endregion
Game::Game() {
	hasPlayer = false;
	hasSkybox = false;
	gameRunning = false;
	lastFrameTime = -1.0f;
	Game::mainCamera = new Camera({ 0, 2, 0 });
	Game::mouseData = { 0, 0, -90 };
	GameConfig::setup(0, { 1, 1, 1 });
}
Game::Game(GLboolean hasPlayer, GLboolean hasSkybox) {
	this->hasPlayer = hasPlayer;
	this->hasSkybox = hasSkybox;
	setupPlayer();
	gameRunning = false;
	Game::mouseData = { 0, 0, -90 };
	lastFrameTime = -1.0f;
	GameConfig::setup(0, { 0.5, 0.5, 0.5 });
	if (hasSkybox) {
		makeSkybox("skybox");
	}
	createCrossHair();
}

void Game::generateWorld() {
	world = World(1, 1, 0);
}
void Game::doLoop(glm::mat4 projection) {
	gameRunning = true;
	setupEventCB(window);
	this->projection = projection;
	mainCamera->setPosition({ 0.0f, -0.0f, 0.0f });
	while (gameRunning) {
		calcTimes();
		lockFPS();
		showFPS();
		proccesEvents();
		doMovement();

		glClearColor(GameConfig::backgroundCol.r, GameConfig::backgroundCol.g, GameConfig::backgroundCol.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		showStuff();

		if (glfwWindowShouldClose(window)) gameRunning = false;

		glfwSwapBuffers(window);
	}
	cleanUp();
}
void Game::calcTimes() {
	GLfloat frame = glfwGetTime();
	deltaTime = frame - lastFrameTime;
	/*if (lastFrameTime == -1)*/ deltaTime = 1.0f / 60.0f;
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
void Game::showStuff() {
	Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
	if (hasPlayer) {
		player.render(projection);
	}
	world.render(cam, projection);
	if (hasSkybox) {
		showSkybox();
	}
	showCrossHair();
}
void Game::setWindow(GLFWwindow* window) {
	this->window = window;
}
void Game::setupPlayer() {
	player = Player({ -2.0f, 5.0f, -0.0f }, { 0.0f, 1.25f, 0.0f } /*{ -0.5f, 0.25f, 3.0f }*/);
	player.create();
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
	if (Game::hasPlayer) {
		Game::player.updateCamera(xOffset, yOffset);
		// Game::player.processMouse(xOffset, yOffset, mouseData.x);
		return;
	}
	Game::mainCamera->ProcessMouseMovement(xOffset, yOffset);
}
void Game::clickCallBack(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
		Game::world.breakBlock(cam.GetPosition(), cam.GetFront());
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
		Game::world.placeBlock(cam.GetPosition(), cam.GetFront());
	}
}
void Game::setupEventCB(GLFWwindow* window) {
	glfwSetKeyCallback(window, Game::keyCallBack);
	glfwSetMouseButtonCallback(window, Game::clickCallBack);
	glfwSetCursorPosCallback(window, Game::mouseCallBack);
}
GLboolean alt = 0;
void Game::doMovement() {
	auto& k = Game::keys;
	GLfloat speed = 9.0f;
	if (k[GLFW_KEY_LEFT_CONTROL]) {
		speed = 12.0f;
		player.setMovementSpeed(PLAYER_SPEED * 2.5f);
	}
	else {
		speed = 2.0f;
		player.setMovementSpeed(PLAYER_SPEED);
	}
	if (Game::hasPlayer ) {
		player.setVelocity({ 0, player.getVelocity().y, 0 });
		if (k[GLFW_KEY_LEFT_ALT]) {
			alt = !alt;
		}
		if (alt || 0) {
			if (k[GLFW_KEY_W]) {
				player.move(Move_Dir::FORWARD);
			}
			if (k[GLFW_KEY_S]) {
				player.move(Move_Dir::BACKWARD);
			}
			if (k[GLFW_KEY_A]) {
				player.move(Move_Dir::LEFT);
			}
			if (k[GLFW_KEY_D]) {
				player.move(Move_Dir::RIGHT);
			}
			if (k[GLFW_KEY_SPACE]) {
				player.move(Move_Dir::UP);
			}
		}
		else {
			auto& c = player.getCamera();
			if (k[GLFW_KEY_W]) {
				c.GetPosition() += c.GetFront() * glm::vec3(1, 0, 1) *  speed * deltaTime;
			}
			if (k[GLFW_KEY_S]) {
				c.GetPosition() -= c.GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_A]) {
				c.GetPosition() -= c.GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_D]) {
				c.GetPosition() += c.GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
			}
			if (k[GLFW_KEY_SPACE]) {
				c.GetPosition() += glm::vec3(0, 1, 0) * speed * deltaTime;
			}
			if (k[GLFW_KEY_LEFT_SHIFT]) {
				c.GetPosition() += glm::vec3(0, -1, 0) * speed * deltaTime;
			}
		}
		

		player.updatePosition(Game::deltaTime, world);
	}
	else {
		if (k[GLFW_KEY_W]) {
			Game::mainCamera->GetPosition() += Game::mainCamera->GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_S]) {
			Game::mainCamera->GetPosition() -= Game::mainCamera->GetFront() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_A]) {
			Game::mainCamera->GetPosition() -= Game::mainCamera->GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_D]) {
			Game::mainCamera->GetPosition() += Game::mainCamera->GetRight() * glm::vec3(1, 0, 1) * speed * deltaTime;
		}
		if (k[GLFW_KEY_SPACE]) {
			Game::mainCamera->GetPosition() += glm::vec3(0, 1, 0) * speed * deltaTime;
		}
		if (k[GLFW_KEY_LEFT_SHIFT]) {
			Game::mainCamera->GetPosition() += glm::vec3(0, -1, 0) * speed * deltaTime;
		}
	}
	/*if (k[GLFW_KEY_F1]) {
		auto start = std::chrono::high_resolution_clock::now();
		world.createChunk({ 32, 16 });
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "made in: " << duration.count() << " microsecconds\n";
	}*/
	Camera& cam = hasPlayer ? player.getCamera() : *mainCamera;
	world.updatePlayerPos(&cam.GetPosition());
}
void Game::cleanUp() {
	for (auto& mesh : FACES) {
		mesh->destroy();
	}
	for (auto& tex : TEXTURES) {
		tex->destroy();
	}
	for (auto& shader : SHADERS) {
		shader->destroy();
	}
}

void Game::makeSkybox(std::string skybox) {
	GLfloat skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	GLuint skyboxVBO;
	glGenVertexArrays(1, &SBVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(SBVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	auto& shader = SHADERS[SKYBOX];
	shader->bind();
	shader->setValue("skybox", 0);
}
void Game::showSkybox() {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	SHADERS[SKYBOX]->bind();
	Camera& camera = hasPlayer ? Game::player.getCamera() : *Game::mainCamera;
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	SHADERS[SKYBOX]->setValue("view", view);
	SHADERS[SKYBOX]->setValue("projection", projection);

	glBindVertexArray(SBVAO);
	TEXTURES[(int)Texture_Names::SKYBOX]->bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDepthFunc(GL_LESS); // set depth function back to default
}
void Game::createCrossHair() {
	// texture 
	crossHair = Texture("crosshair", 1);
	crossHair.load2D(crossHair.getName());
	// 
	// vao/vbo
	GLfloat positions[] = {
		0.25f,  0.45f, 0.0f,  1, 1,
		0.25f, -0.45f, 0.0f,  1, 0,
		-0.25f,  0.45f, 0.0f, 0, 1,

		0.25f, -0.45f, 0.0f,  1, 0,
		-0.25f, -0.45f, 0.0f, 0, 0,
		-0.25f,  0.45f, 0.0f, 0, 1
	};
	GLuint vbo;
	glGenVertexArrays(1, &CHVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(CHVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	auto& shader = SHADERS[CROSSHAIR];
	shader->bind();
	crossHair.bind();
	auto scale = glm::vec3(0.1f);
	shader->setValue("scale", scale);
	shader->setValue("alpha", 1);
	shader->setValue("texture1", 0);
	shader->unBind();
}
void Game::showCrossHair() {
	auto& shader = SHADERS[CROSSHAIR];
	shader->bind();
	crossHair.bind();

	glBindVertexArray(CHVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	shader->unBind();
	crossHair.unBind();
}
