#pragma once

#include "Utils.h"
#include <PxPhysicsAPI.h>
#include <characterkinematic/PxController.h>

using namespace std;
using namespace physx;


class Player {

public:

	bool jumping = false;
	float jumpVelocity = 0.25f;
	bool jumpGoingUp = true;
	float cappedHeight = 10.0f;
	PxExtended currentHeight = 0.0f;

	PxPhysics* gPhysics;
	PxScene* gScene;
	PxControllerManager* gCCTManager = nullptr;
	PxController* gPlayerController = nullptr;
	PxControllerCollisionFlags collFlags;

	Player();

	Player(PxPhysics* gPhysics, PxScene* gScene);

	~Player();

	void init();

	void update();

	void draw();

	void reset();

	void moveChar(glm::vec3 displacement, float deltaTime, PxControllerFilters filter = nullptr);

	void jump(float deltaTime);


private:

	float gGravity = -9.81f;
	PxF32 pHeight = PxF32(1.4f);
	PxF32 pRadius = PxF32(0.6f);
	PxExtendedVec3 pStartPos = PxExtendedVec3(-3.0f, pHeight, 0.0f);

};