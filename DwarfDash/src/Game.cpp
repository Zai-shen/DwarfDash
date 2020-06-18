#include "Game.h"

using namespace std;

Game::Game() {}

Game::~Game() {
	//cout << "destroying game variables" << endl;
	player->~Player();
	currentLevel->~Level();
	ground->release();
}

void Game::init() {
	// Load shaders
	initShaders();

	// Create textures
	initTextures();

	// Create materials
	initMaterials();

	// Init defaults
		//static friction, dynamic friction, restitution
	defaultMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
	defaultPlatGeometry = new PxBoxGeometry(PxVec3(platDefaultWidth/2, platDefaultWidth/8, platDefaultWidth/2));
	defaultPickUpGeometry = new PxSphereGeometry(PxReal(1.5f));

	// Create Geometry
	initLevels();

	// Create & init Player
	player = new Player(gPhysics, gScene);

	// Create & init particle system
	//particleSystem = new ParticleSystem(particleShader, 50);
	particleGenerator = new ParticleGenerator(particleShader, camPointer, 1000, glm::vec3(-1.5f, 2.f, 1.5f));

	// Start game
	currentGameState = GAME_STATE_ACTIVE;
}

void Game::initShaders() {
	primaryShader = make_shared<Shader>("texture.vert", "texture.frag");
	modelShader = make_shared<Shader>("modelloading.vert", "modelloading.frag");
	skyboxShader = make_shared<Shader>("skybox.vert", "skybox.frag");
	//particleShader = make_shared<Shader>("particles.vert", "particles.frag");
	particleShader = make_shared<Shader>("particlesInstanced.vert", "particlesInstanced.frag");
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
	//1-Creating static plane
	createGroundPlane();

	addPlatformLine(5, F, PxVec3(0.f,0.f,0.f));
	addPlatformLine(3, R, 5 * platSpacingFront);
	addPlatformLine(2, B, 5 * platSpacingFront + 3 * platSpacingRight);

	// Coin
	Gameobject* goCoin = new Gameobject(new Model("assets/models/coin/Coin_low_poly_colored.obj", primaryShader));
	addGameobject(goCoin, true, PxVec3(0.f,1.6f,0.f) + 5 * platSpacingFront + 3 * platSpacingRight + 1 * platSpacingBack, *defaultPickUpGeometry, "coin");

	addPlatformStairs(5, F, 5 * platSpacingFront + 3 * platSpacingRight + 1 * platSpacingFront);
	addPlatformLine(5, R, 5 * platSpacingFront + 3 * platSpacingRight + 1 * platSpacingFront + 5 * platSpacingFront + 2 * platSpacingLeft);
	addPlatformLine(5, R, 5 * platSpacingFront + 3 * platSpacingRight + 1 * platSpacingFront + 5 * platSpacingFront + 2 * platSpacingLeft + 1 * platSpacingFront);
	addPlatformLine(5, R, 5 * platSpacingFront + 3 * platSpacingRight + 1 * platSpacingFront + 5 * platSpacingFront + 2 * platSpacingLeft + 2 * platSpacingFront);
	addPlatformLine(5, R, 5 * platSpacingFront + 3 * platSpacingRight + 1 * platSpacingFront + 5 * platSpacingFront + 2 * platSpacingLeft + 3 * platSpacingFront);

	addGameobject(new Gameobject(new Model("assets/models/plattform/plattform_normal.obj", primaryShader)),
		false, 15 * platSpacingFront + 3 * platSpacingRight + platCurrentHeight, *defaultPlatGeometry);

	addGameobject(new Gameobject(new Model("assets/models/plattform/plattform_normal.obj", primaryShader)),
		false, 17 * platSpacingFront + 3 * platSpacingRight + platCurrentHeight, *defaultPlatGeometry);

	addGameobject(new Gameobject(new Model("assets/models/plattform/plattform_normal.obj", primaryShader)),
		false, 19 * platSpacingFront + 3 * platSpacingRight + platCurrentHeight, *defaultPlatGeometry);

	//Moving platform
	addGameobject(new Gameobject(new Model("assets/models/plattform/plattform_normal.obj", primaryShader)),
		true, 22 * platSpacingFront + 2 * platSpacingRight + platCurrentHeight, *defaultPlatGeometry, "platformMoving");

	addPlatformLine(3, R, 24 * platSpacingFront + 2 * platSpacingRight);

	//Goal
	addGameobject(new Gameobject(new Model("assets/models/goal/Mine_escape_low_poly_colored.obj", primaryShader)),
		false, 24 * platSpacingFront + 3 * platSpacingRight + platCurrentHeight + PxVec3(0.f,5.f,0.f), PxBoxGeometry (PxVec3(7.5f, 5.f, .5f)), "goal");

	//Death cloud
	addGameobject(new Gameobject(new Model("assets/models/cloud/cloud.obj", primaryShader)),
		true, 5 * platSpacingBack + PxVec3(0,6,0), PxBoxGeometry(PxVec3(50.f, 5.f, 2.5f)), "cloud");
}

void Game::initLevel2() {
	createGroundPlane();
	addPlatformLine(5, F, PxVec3(0.f, 0.f, 0.f));

	// Dynamic coin example
	Model* coin = new Model("assets/models/coin/Coin_low_poly_colored.obj", primaryShader);
	Gameobject* goCoin = new Gameobject(coin);
	addGameobject(goCoin, true, PxVec3(0.0f, 5.0f, -10.0f), *defaultPickUpGeometry, "coin");

	// Dynamic heart example
	Model* heart = new Model("assets/models/heart/Heart_low_poly_colored.obj", primaryShader);
	Gameobject* goHeart = new Gameobject(heart);
	addGameobject(goHeart, true, PxVec3(5.0f, 5.0f, -10.0f), *defaultPickUpGeometry, "heart");

	// Dynamic shield example
	Model* shield = new Model("assets/models/shield/Shield_low_poly_colored.obj", primaryShader);
	Gameobject* goShield = new Gameobject(shield);
	addGameobject(goShield, true, PxVec3(-5.0f, 5.0f, -10.0f), *defaultPickUpGeometry, "shield");

	// Static actor example
	Model* goal = new Model("assets/models/goal/Mine_escape_low_poly_colored.obj", primaryShader);
	Gameobject* goGoal = new Gameobject(goal);
	addGameobject(goGoal, false, PxVec3(0.0f, 5.f, -20.0f), PxBoxGeometry(PxVec3(7.5f, 5.f, .5f)), "goal");

	// Static actor example
	Model* platNorm = new Model("assets/models/plattform/plattform_normal.obj", primaryShader);
	Gameobject* goPlatNorm = new Gameobject(platNorm);
	addGameobject(goPlatNorm, false, PxVec3(-10.0f, 2.5f, .0f), *defaultPlatGeometry, "platformNormal");

	// Static actor example
	Model* platTorch = new Model("assets/models/plattform/Platform_Torch.obj", primaryShader);
	Gameobject* goPlatTorch = new Gameobject(platTorch);
	addGameobject(goPlatTorch, false, PxVec3(-10.0f, 2.5f, -10.0f), *defaultPlatGeometry, "platformTorch");
}

void Game::initLevel3() {
	createGroundPlane();
	addPlatformLine(50, F, PxVec3(0));

	Gameobject* goGoal = new Gameobject(new Model("assets/models/goal/Mine_escape_low_poly_colored.obj", primaryShader));
	addGameobject(goGoal, false, 50 * platSpacingFront, PxBoxGeometry(PxVec3(7.5f, 5.f, .5f)), "goal");
}

void Game::createGroundPlane() {
	PxTransform planePos = PxTransform(PxVec3(0.0f, -5.f,
		0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	ground = gPhysics->createRigidStatic(planePos);
	PxShape* shape = gPhysics->createShape(PxPlaneGeometry(), *defaultMaterial);
	shape->setName("ground");
	ground->setName("ground");
	ground->attachShape(*shape);
	gScene->addActor(*ground);
}

void Game::addPlatformLine(int length, Direction direction, PxVec3 startingPosition) {
	PxVec3 spacing = PxVec3(0.f,0.f,0.f);
	
	if (direction == F){
		spacing += platSpacingFront;
	}
	if (direction == B) {
		spacing += platSpacingBack;
	}
	if (direction == L) {
		spacing += platSpacingLeft;
	}
	if (direction == R) {
		spacing += platSpacingRight;
	}

	for (int i = 0; i < length; i++)
	{
		if (i == 0) {
			addGameobject(new Gameobject(new Model("assets/models/plattform/Platform_Torch.obj", primaryShader)), false,
				startingPosition + platCurrentHeight + (spacing * (float)i), *defaultPlatGeometry, "platformTorch");
		}
		else {
			addGameobject(new Gameobject(new Model("assets/models/plattform/plattform_normal.obj", primaryShader)), false,
				startingPosition + platCurrentHeight + (spacing * (float)i), *defaultPlatGeometry, "platformNormal");
		}
	}
}

void Game::addPlatformStairs(int length, Direction direction, PxVec3 startingPosition) {
	PxVec3 spacing = platCurrentHeight;

	if (direction == F) {
		spacing += platSpacingFront;
	}
	if (direction == B) {
		spacing += platSpacingBack;
	}
	if (direction == L) {
		spacing += platSpacingLeft;
	}
	if (direction == R) {
		spacing += platSpacingRight;
	}

	for (int i = 0; i < length; i++)
	{
		if (i == 0) {
			addGameobject(new Gameobject(new Model("assets/models/plattform/Platform_Torch.obj", primaryShader)), false,
				startingPosition + platCurrentHeight + (spacing * (float)i), *defaultPlatGeometry, "platformTorch");
		}
		else {
			platCurrentHeight += inclination;
			addGameobject(new Gameobject(new Model("assets/models/plattform/plattform_normal.obj", primaryShader)), false,
				startingPosition + platCurrentHeight + (spacing * (float)i), *defaultPlatGeometry, "platformNormal");
		}
	}
}

void Game::update(float dt) {
	currentLevel->update(dt);
	player->update(dt);
	//particleSystem->Update(dt, glm::vec3(0.f,1.f,0.f), 2, glm::vec3(0.f,0.f,0.f));
	particleGenerator->update(dt);

	if (player->hasLost)
	{
		player->hasLost = false;

		cout << "You lost!" << endl;
		cout << "Score: " << player->score << endl;

		reset();
	}

	if (player->hasWon)
	{
		player->hasWon = false;
		platCurrentHeight = PxVec3(0.0f);
		player->setToStartPosition();

		cout << "You win!" << endl;
		cout << "Score: " << player->score << endl;

		ground->release();
		currentLevel->~Level();
		currentLevel = nextLevel();
		initLevels();
	}
}

void Game::draw() {
	currentLevel->draw();
	player->draw();
	//particleSystem->Draw(camPointer->getViewProjectionMatrix(), camPointer->getPosition());
	particleGenerator->draw();

}

void Game::reset() {
	platCurrentHeight = PxVec3(0.0f);
	player->reset();
	currentLevel->reset();
	//currentLevel->~Level();
	//initLevels();
}

void Game::addGameobject(Gameobject* gameObject, bool dynamic, PxVec3 position, PxGeometryHolder geometry, const char* name) {
	if (!gameObject->goMaterial) {
		gameObject->goMaterial = defaultMaterial;
	}
	
	gameObject->goPosition = PxTransform(position);
	gameObject->goStartPosition = PxTransform(position);

	if (dynamic)
	{
		gameObject->goDynamicActor = PxCreateDynamic(*gPhysics, gameObject->goPosition, geometry.any(), *(gameObject->goMaterial), 1.0f);
		gameObject->goDynamicActor->setName(name);

		// Let powerups rotate
		if (name == "coin" || name == "heart" || name == "shield")
		{
			gameObject->goDynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			gameObject->goDynamicActor->setAngularVelocity(PxVec3(0.f, 2.f, 0.f));
			gameObject->goDynamicActor->setAngularDamping(0.f);
		}
		// Let moving platforms be kinematic actors
		else if (name == "platformMoving") {
			gameObject->goDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		}
		// Let the death cloud move towards the player
		else if (name == "cloud") {
			gameObject->goDynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			gameObject->goDynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		}
		gScene->addActor(*(gameObject->goDynamicActor));
	}
	else {
		gameObject->goActor = PxCreateStatic(*gPhysics, gameObject->goPosition, geometry.any(), *(gameObject->goMaterial));
		gameObject->goActor->setName(name);

		gScene->addActor(*(gameObject->goActor));
	}

	currentLevel->addGameobject(gameObject);
}

Level* Game::getCurrentLevel() {
	return this->currentLevel;
}

Level* Game::nextLevel() {
	if (currentLevel == level1)
	{
		return level2;
	}
	else if (currentLevel = level2) {
		return level3;
	}
	else if (currentLevel = level3) {
		cout << "Congratulations! You won the entire game!" << endl;
		return level1;
	}
}
