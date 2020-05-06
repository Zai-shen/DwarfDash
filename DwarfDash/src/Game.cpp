#include "Game.h"

using namespace std;

Game::Game() {}

Game::~Game() {}

void Game::init() {
	// Load shader(s)
	primaryShader = make_shared<Shader>("texture.vert", "texture.frag");

	// Create textures
	woodTexture = make_shared<Texture>("wood_texture.dds");
	brickTexture = make_shared<Texture>("bricks_diffuse.dds");

	// Create materials
	woodTextureMaterial = make_shared<TextureMaterial>(primaryShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
	brickTextureMaterial = make_shared<TextureMaterial>(primaryShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, brickTexture);
}

void Game::update() {
	this->currentLevel->update();
}

void Game::draw() {
	this->currentLevel->draw();
}

void Game::reset() {
	this->currentLevel->reset();
	createInitialGeometry();
}

void Game::createInitialGeometry() {
	//// Create geometry
	//Geometry cube = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f), woodTextureMaterial);
	//TIL: Pointer to objects last longer in memory than simple objects do!
	Geometry* platform_1 = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(8.0f, 1.f, 8.f), woodTextureMaterial);
	Geometry* cylinder = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 2.0f, 6.5f)), Geometry::createCylinderGeometry(32, 1.3f, 1.0f), brickTextureMaterial);
	Geometry* sphere = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 0.0f)), Geometry::createSphereGeometry(64, 32, 1.0f), brickTextureMaterial);

	cylinder->transform(glm::scale(glm::mat4(1.0f), glm::vec3(.5f, .5f, .5f)));

	this->currentLevel->levelObjects.push_back(platform_1);
	this->currentLevel->levelObjects.push_back(cylinder);
	this->currentLevel->levelObjects.push_back(sphere);

	//Can we later on access & manipulate objects? like turn them? if we add them like this:
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -14.0f)), Geometry::createCubeGeometry(8.0f, 1.f, 8.f), woodTextureMaterial));
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 3.f, 0.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), brickTextureMaterial));
	addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-4.f, 1.f, 4.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), woodTextureMaterial));
	//game->reset();
}

void Game::addGeometry(Geometry* geometry) {
	this->currentLevel->levelObjects.push_back(geometry);
}