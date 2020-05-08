#include "Game.h"

using namespace std;

Game::Game() {}

Game::~Game() {}

void Game::init() {
	// Load shaders
	this->initShaders();

	// Create textures
	this->initTextures();

	// Create materials
	this->initMaterials();

	// Create Geometry
	this->initGeometry();
}

void Game::initShaders() {
	primaryShader = make_shared<Shader>("texture.vert", "texture.frag");
	modelShader = make_shared<Shader>("modelloading.vert", "modelloading.frag");
}

void Game::initTextures() {
	woodTexture = make_shared<Texture>("wood_texture.dds");
	brickTexture = make_shared<Texture>("bricks_diffuse.dds");
}

void Game::initMaterials() {
	woodTextureMaterial = make_shared<TextureMaterial>(primaryShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
	brickTextureMaterial = make_shared<TextureMaterial>(primaryShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, brickTexture);
}

void Game::initGeometry() {
	if (currentLevel == level1)
	{
		this->initLevel1();
	}
	else {
		this->initLevel2();
	}
}

void Game::initLevel1() {
	//// Create geometry
	//Geometry cube = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f), woodTextureMaterial);
	Geometry* platform_1 = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(8.0f, 1.f, 8.f), woodTextureMaterial);
	Geometry* cylinder = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 2.0f, 6.5f)), Geometry::createCylinderGeometry(32, 1.3f, 1.0f), brickTextureMaterial);
	Geometry* sphere = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 0.0f)), Geometry::createSphereGeometry(64, 32, 1.0f), brickTextureMaterial);

	this->currentLevel->addGeometry(platform_1);
	this->currentLevel->addGeometry(cylinder);
	this->currentLevel->addGeometry(sphere);

	//Can we later on access & manipulate objects? like turn them? if we add them like this:
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -14.0f)), Geometry::createCubeGeometry(8.0f, 1.f, 8.f), woodTextureMaterial));
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 3.f, 0.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), brickTextureMaterial));
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-4.f, 1.f, 4.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), woodTextureMaterial));
}

void Game::initLevel2() {
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(8.0f, 1.f, 8.f), brickTextureMaterial));
}

void Game::update() {
	this->currentLevel->update();
}

void Game::draw() {
	this->currentLevel->draw();
}

void Game::reset() {
	this->currentLevel->reset();
	initGeometry();
}

void Game::addGeometry(Geometry* geometry) {
	this->currentLevel->levelObjects.push_back(geometry);
}

Level* Game::getCurrentLevel() {
	return this->currentLevel;
}