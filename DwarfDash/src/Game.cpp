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
	initLevels();
}

void Game::initShaders() {
	primaryShader = make_shared<Shader>("texture.vert", "texture.frag");
	modelShader = make_shared<Shader>("modelloading.vert", "modelloading.frag");
	lightShader = make_shared<Shader>("light.vert", "light.frag");
}

void Game::initTextures() {
	woodTexture = make_shared<Texture>("wood_texture.dds");
	brickTexture = make_shared<Texture>("bricks_diffuse.dds");
}

void Game::initMaterials() {
	woodTextureMaterial = make_shared<TextureMaterial>(primaryShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
	brickTextureMaterial = make_shared<TextureMaterial>(primaryShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, brickTexture);
}

void Game::initLevels() {
	if (currentLevel == level1)
	{
		initLevel1();
	}
	else if (currentLevel == level2) {
		initLevel2();
	}else if (currentLevel == level3) {
		initLevel3();
	}else {
		cout << "No level initialized" << endl;
	}
}

void Game::initLevel1() {
	PxMaterial* standardMaterial =
		//static friction, dynamic friction, restitution
		gPhysics->createMaterial(0.5, 0.5, 0.5);

	//1-Creating static plane
	PxTransform planePos = PxTransform(PxVec3(0.0f, 0,
		0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysics->createRigidStatic(planePos);
	PxShape* shape = gPhysics->createShape(PxPlaneGeometry(), *standardMaterial);
	plane->attachShape(*shape);
	
	gScene->addActor(*plane);


	Model* mod1 = new Model("assets/models/plattform/plattform_brick.obj", primaryShader);
	//Model* mod1 = new Model("assets/models/nanosuit/nanosuit.obj", primaryShader);
	Gameobject* model1 = new Gameobject(mod1);
	PxBoxGeometry tempBoxGeometry(PxVec3(2.5f, 2.5f, 2.5f)); //this->model
	model1->goMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	model1->goPosition = PxTransform(PxVec3(5.0f, 25.0f, 5.0f)); // should be geometry.getPos
	model1->goDynamicActor = PxCreateDynamic(*gPhysics, model1->goPosition, tempBoxGeometry, *(model1->goMaterial), 1.0f);

	gScene->addActor(*(model1->goDynamicActor));
	addGameobject(model1);

	Model* mod2 = new Model("assets/models/plattform/plattform.obj", primaryShader);
	//Model* mod2 = new Model("assets/models/plattform/plattform.obj", primaryShader);
	Gameobject* model2 = new Gameobject(mod2);
	model2->goMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	model2->goPosition = PxTransform(PxVec3(-5.0f, 50.0f, -5.0f)); // should be geometry.getPos
	model2->goDynamicActor = PxCreateDynamic(*gPhysics, model2->goPosition, tempBoxGeometry, *(model2->goMaterial), 1.0f);

	gScene->addActor(*(model2->goDynamicActor));
	addGameobject(model2);
}

void Game::initLevel2() {
	PxMaterial* standardMaterial =
		//static friction, dynamic friction, restitution
		gPhysics->createMaterial(0.5, 0.5, 0.5);

	//1-Creating static plane
	PxTransform planePos = PxTransform(PxVec3(0.0f, 0,
		0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysics->createRigidStatic(planePos);
	PxShape* shape = gPhysics->createShape(PxPlaneGeometry(), *standardMaterial);
	plane->attachShape(*shape);
	gScene->addActor(*plane);


	Model* mod2 = new Model("assets/models/backpack/backpack.obj", modelShader);
	Gameobject* model2 = new Gameobject(mod2);
	PxBoxGeometry tempBackGeometry(PxVec3(2.5f, 2.5f, 2.5f)); //this->model
	model2->goMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	model2->goPosition = PxTransform(PxVec3(-2.0f, 50.0f, 0.0f)); // should be geometry.getPos
	model2->goDynamicActor = PxCreateDynamic(*gPhysics, model2->goPosition, tempBackGeometry, *(model2->goMaterial), 1.0f);

	gScene->addActor(*(model2->goDynamicActor));
	addGameobject(model2);
}

void Game::initLevel3() {

}


void Game::update() {
	currentLevel->update();
}

void Game::draw() {
	currentLevel->draw();
}

void Game::reset() {
	currentLevel->reset();
	initLevels();
}

void Game::addGameobject(Gameobject* gameObject) {
	currentLevel->addGameobject(gameObject);
}

Level* Game::getCurrentLevel() {
	return this->currentLevel;
}