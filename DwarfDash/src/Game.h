#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Geometry.h"
#include "Level.h"
#include "Gameobject.h"
#include "Model.h"
#include <PxPhysicsAPI.h>
#include "Player.h"
#include "ParticleGenerator.h"
#include "FPSCamera.h"
#include "FlareRenderer.h"

enum Direction {
	F,
	B,
	L,
	R
};

using namespace std;

class Game {

public:
	int const GAME_STATE_IDLE = 0;
	int const GAME_STATE_ACTIVE = 1;
	int const GAME_STATE_PAUSED = 2;
	int const GAME_STATE_FINISHED = 3;

	int currentGameState = GAME_STATE_IDLE;

	shared_ptr<Shader> primaryShader;
	shared_ptr<Shader> modelShader;
	shared_ptr<Shader> skyboxShader;
	shared_ptr<Shader> particleShader;
	shared_ptr<Shader> flareShader;

	shared_ptr<Material> woodTextureMaterial;
	shared_ptr<Material> brickTextureMaterial;

	PxPhysics* gPhysics;
	PxScene* gScene;
	PxRigidStatic* ground;

	Player* player;
	vector<ParticleGenerator*> particleSystem;
	FPSCamera* camPointer;
	FlareRenderer* flareRenderer;

	Game();
	~Game();

	void init();

	void update(float dt);

	Level* nextLevel();

	void draw();

	void reset();

	void addGameobject(Gameobject* gameObject, bool dynamic = false, PxVec3 position = PxVec3(0.f, 0.f, 0.f),
		PxGeometryHolder geometry = PxBoxGeometry(PxVec3(2.f, 2.f, 2.f)), const char* name = "noName");

	Level* getCurrentLevel();

	std::string getLevelString();

private:

	Level* level1 = new Level();
	Level* level2 = new Level();
	Level* level3 = new Level();
	Level* currentLevel = level1;
	//Level* currentLevel = level2;

	shared_ptr<Texture> woodTexture;
	shared_ptr<Texture> brickTexture;

	void initShaders();
	void initTextures();
	void initMaterials();
	void initLevels();
	void initLevel1();
	void initLevel2();
	void initLevel3();
	void addPlatformLine(int length, Direction direction, PxVec3 startingPosition);
	void addPlatformStairs(int length, Direction direction, PxVec3 startingPosition);
	void createGroundPlane();



	PxVec3 platCurrentHeight = PxVec3(0.f,0.f,0.f);
	PxVec3 inclination = PxVec3(0.f, 1.f, 0.f);
	float platDefaultWidth = 4.f;
	PxVec3 platSpacingFront = PxVec3(0.f, 0.f, -4.f);
	PxVec3 platSpacingBack = PxVec3(0.f, 0.f, 4.f);
	PxVec3 platSpacingLeft = PxVec3(-4.f, 0.f, 0.f);
	PxVec3 platSpacingRight = PxVec3(4.f, 0.f, 0.f);

	PxMaterial* defaultMaterial;
	PxBoxGeometry* defaultPlatGeometry;
	PxSphereGeometry* defaultPickUpGeometry;
};