#include "Game.h"

using namespace std;

Game::Game() {}

Game::~Game() {
	player->~Player();
	cout << "destroying game variables" << endl;
	currentLevel->~Level();
}

void Game::init() {
	// Load shaders
	initShaders();

	// Create textures
	initTextures();

	// Create materials
	initMaterials();

	// Init PhysX materials
		//static friction, dynamic friction, restitution
	standardMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);

	// Create Geometry
	initLevels();

	// Create & init Player
	player = new Player(gPhysics, gScene);

	// Start game
	currentGameState = GAME_STATE_ACTIVE;
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

void Game::initLevels() {
	if (currentLevel == level1)
	{
		initLevel1();
	}
	else {
		initLevel2();
	}
}

void Game::createGroundPlane() {
	PxTransform planePos = PxTransform(PxVec3(0.0f, 0,
		0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysics->createRigidStatic(planePos);
	PxShape* shape = gPhysics->createShape(PxPlaneGeometry(), *standardMaterial);
	plane->attachShape(*shape);
	gScene->addActor(*plane);
}


void Game::initLevel1() {
	//Trying a model gameobject
	//Model mo1("assets/models/plattform/plattform.obj", this->modelShader);
	Model* mod1 = new Model("assets/models/plattform/plattform.obj", modelShader);
	Gameobject* model1 = new Gameobject(mod1);
	PxBoxGeometry tempBoxGeometry(PxVec3(0.5f, 0.5f, 0.5f)); //this->model
	model1->goMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	model1->goPosition = PxTransform(PxVec3(0.0f, 0.0f, 0.0f)); // should be geometry.getPos
	model1->goDynamicActor = PxCreateDynamic(*gPhysics, model1->goPosition, tempBoxGeometry, *(model1->goMaterial), 1.0f);

	gScene->addActor(*(model1->goDynamicActor));
	addGameobject(model1);

	Model* mod2 = new Model("assets/models/nanosuit/nanosuit.obj", modelShader);
	Gameobject* model2 = new Gameobject(mod2);
	model2->goMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	model2->goPosition = PxTransform(PxVec3(0.0f, 5.0f, 0.0f)); // should be geometry.getPos
	model2->goDynamicActor = PxCreateDynamic(*gPhysics, model2->goPosition, tempBoxGeometry, *(model2->goMaterial), 1.0f);

	gScene->addActor(*(model2->goDynamicActor));
	addGameobject(model2);
}

void Game::initLevel2() {
	//1-Creating static plane
	createGroundPlane();

	//Gameobject* box1 = new Gameobject(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-1.f, 5.f, 0.f)), Geometry::createSphereGeometry(64, 32, 1.0f), brickTextureMaterial));
	Gameobject* box1 = new Gameobject(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 5.f, 0.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), brickTextureMaterial));
	PxBoxGeometry tempBoxGeometry(PxVec3(.5f, .5f, .5f)); //this->model
	box1->goMaterial = standardMaterial;
	box1->goPosition = PxTransform(PxVec3(0.0f, 5.0f, 0.0f)); // should be geometry.getPos
	box1->goDynamicActor = PxCreateDynamic(*gPhysics, box1->goPosition, tempBoxGeometry, *(box1->goMaterial), 1.0f);

	gScene->addActor(*(box1->goDynamicActor));
	addGameobject(box1);


	Gameobject* sphere1 = new Gameobject(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 15.f, 0.f)), Geometry::createSphereGeometry(64, 32, 1.0f), woodTextureMaterial));
	PxSphereGeometry tempSphereGeometry(PxReal(1.0f)); //this->model
	sphere1->goMaterial = standardMaterial;
	sphere1->goPosition = PxTransform(PxVec3(0.0f, 15.0f, 0.0f)); // should be geometry.getPos
	sphere1->goDynamicActor = PxCreateDynamic(*gPhysics, sphere1->goPosition, tempSphereGeometry, *(sphere1->goMaterial), 1.0f);

	gScene->addActor(*(sphere1->goDynamicActor));
	addGameobject(sphere1);


	Gameobject* plat1 = new Gameobject(new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-3.f, 5.f, 0.f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), brickTextureMaterial));
	PxBoxGeometry tempPlatGeometry(PxVec3(.5f, .5f, .5f)); //this->model
	plat1->goMaterial = standardMaterial;
	plat1->goPosition = PxTransform(PxVec3(-3.0f, 5.0f, 0.0f)); // should be geometry.getPos
	plat1->goActor = PxCreateStatic(*gPhysics, plat1->goPosition, tempPlatGeometry, *(plat1->goMaterial));
	
	gScene->addActor(*(plat1->goActor));
	addGameobject(plat1);
}

void Game::update() {
	currentLevel->update();
	player->update();
}

void Game::draw() {
	currentLevel->draw();
	player->draw();
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