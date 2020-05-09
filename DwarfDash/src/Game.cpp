#include "Game.h"

using namespace std;

Game::Game() {}

Game::~Game() {}

void Game::init() {
	// Load shaders
	initShaders();

	// Create textures
	initTextures();

	// Create materials
	initMaterials();

	// Create Geometry
	initGeometry();
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
		initLevel1();
	}
	else {
		initLevel2();
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
	this->addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -14.0f)), Geometry::createCubeGeometry(8.0f, 1.f, 8.f), woodTextureMaterial));
	this->addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 3.f, 0.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), brickTextureMaterial));
	this->addGeometry(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-4.f, 1.f, 4.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), woodTextureMaterial));
}

void Game::initLevel2() {
	Gameobject* box1 = new Gameobject(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-1.f, 5.f, 0.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), brickTextureMaterial));
	box1->goMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	box1->goPosition = PxTransform(PxVec3(-1.0f, 5.0f, 0.0f)); // should be geometry.getPos
	PxBoxGeometry tempGeometry(PxVec3(0.5f, 0.5f, 0.5f)); //this->model
	//box1->setAsDynamicActor(gPhysics);
	box1->goDynamicActor = PxCreateDynamic(*gPhysics, box1->goPosition, tempGeometry, *(box1->goMaterial), 1.0f);
	gScene->addActor(*(box1->goDynamicActor));
	addGameobject(box1);


	//PxRigidDynamic* cBox;
	//PxMaterial* cMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	//PxTransform cPos(PxVec3(0.0f, 5.0f, -1.0f)); //this->position
	//PxBoxGeometry cGeometry(PxVec3(0.5f, 0.5f, 0.5f)); //this->model
	//cBox = PxCreateDynamic(*gPhysics, cPos, cGeometry, *cMaterial, 1.0f);
	//gScene->addActor(*cBox);


}

void Game::getPhysXHandles(PxPhysics* gPhysics, PxScene* gScene) {
	this->gPhysics = gPhysics;
	this->gScene = gScene;
}

void Game::update() {
	currentLevel->update();
}

void Game::draw() {
	currentLevel->draw();
}

void Game::reset() {
	currentLevel->reset();
	initGeometry();
}

void Game::addGeometry(Geometry* geometry) {
	currentLevel->addGeometry(geometry);
}

void Game::addGameobject(Gameobject* gameObject) {
	currentLevel->addGameobject(gameObject);
}

Level* Game::getCurrentLevel() {
	return this->currentLevel;
}