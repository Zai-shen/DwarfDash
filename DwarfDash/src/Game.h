#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Geometry.h"
#include "Level.h"
#include "Gameobject.h"

using namespace std;

class Game {

public:

	shared_ptr<Shader> primaryShader;
	shared_ptr<Shader> modelShader;
	shared_ptr<Material> woodTextureMaterial;
	shared_ptr<Material> brickTextureMaterial;

	Game();
	~Game();

	void init();

	void update();

	void draw();

	void reset();

	void addGeometry(Geometry* geometry);

	void addGameobject(Gameobject* gameObject);

	Level* getCurrentLevel();

	void getPhysXHandles(PxPhysics* gPhysics, PxScene* gScene);

private:

	Level* level1 = new Level();
	Level* level2 = new Level();
	Level* currentLevel = level2;

	int const GAME_STATE_IDLE = 0;
	int const GAME_STATE_ACTIVE = 1;
	int const GAME_STATE_PAUSED = 2;
	int const GAME_STATE_FINISHED = 3;

	int activeGameState = GAME_STATE_IDLE;

	shared_ptr<Texture> woodTexture;
	shared_ptr<Texture> brickTexture;

	PxPhysics* gPhysics;
	PxScene* gScene;

	void initShaders();
	void initTextures();
	void initMaterials();
	void initGeometry();
	void initLevel1();
	void initLevel2();

};