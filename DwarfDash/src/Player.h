#pragma once

#include "Utils.h"
#include <PxPhysicsAPI.h>


using namespace std;
using namespace physx;


class Player {

public:

	PxPhysics* gPhysics;
	PxScene* gScene;
	PxControllerManager* gCCTManager = nullptr;
	PxController* gPlayerController = nullptr;

	Player();

	Player(PxPhysics* gPhysics, PxScene* gScene);

	~Player();

	void init();

	void update();

	void draw();

	void reset();

	void moveChar(glm::vec3 displacement, float deltaTime, float yPos);

private:

	float gGravity = -9.81f;
};