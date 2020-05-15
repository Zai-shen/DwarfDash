#include "Player.h"

using namespace std;
using namespace physx;


Player::Player() {}


Player::Player(PxPhysics* gPhysics, PxScene* gScene) {
	this->gPhysics = gPhysics;
	this->gScene = gScene;
	init();
}

Player::~Player() {}

void Player::init() {
	//Creating Character Controller Manager
	gCCTManager = PxCreateControllerManager(*gScene);

	//gCCTManager->setOverlapRecoveryModule(true);

	//Character Controller for Player (of type capsule)
	PxCapsuleControllerDesc charDesc;
	charDesc.height = pHeight; //height
	charDesc.radius = pRadius; //from height to top/bottom
	charDesc.position = pStartPos; //starting position
	charDesc.contactOffset = 0.05f; //controller skin width for collisions
	charDesc.stepOffset = 0.01f; //max obstacle climb height
	charDesc.slopeLimit = cosf(glm::radians(45.0f)); // max slope to walk
	charDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // Specifies the 'up' direction
	charDesc.material = gPhysics->createMaterial(0.1f, 0.1f, 0.1f);

	gPlayerController = gCCTManager->createController(charDesc);
}

void Player::update() {
}

void Player::draw() {
	//cout << "Controller pos:" << endl;
	//cout << gPlayerController->getPosition().x << "x " << gPlayerController->getPosition().y << "y " << gPlayerController->getPosition().z << "z " << endl;
}

void Player::reset() {

}

void Player::moveChar(glm::vec3 displacement, float deltaTime, PxControllerFilters filter) {
	collFlags = gPlayerController->move(PxVec3(displacement.x, displacement.y, displacement.z ), 0.01f, deltaTime, filter, nullptr);
}

void Player::jump(float deltaTime) {

	const PxControllerFilters pxCF;

	if (jumping)
	{

		currentHeight = gPlayerController->getPosition().y;

		//jumpVelocity += gGravity/10 * deltaTime;
		//if (jumpVelocity < 0.0f) {
		//	jumpVelocity = 0.0f;
		//}


		if (currentHeight >= cappedHeight)
		{
			jumpGoingUp = false;
		}

		if (jumpGoingUp)
		{
			moveChar(glm::vec3(0.0, -gGravity * deltaTime, 0.0), deltaTime, pxCF);
		}
		else {
			moveChar(glm::vec3(0.0, gGravity * deltaTime, 0.0), deltaTime, pxCF);
		}
		
		if (collFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			jumpGoingUp = true;
			cout << "collision down!" << endl;
		}
		if (collFlags & PxControllerCollisionFlag::eCOLLISION_UP)
		{
			jumpGoingUp = false;
			cout << "collision up!" << endl;
		}
	}
	else {
		jumpVelocity = .1f;
		moveChar(glm::vec3(0.0, gGravity * deltaTime, 0.0), deltaTime, pxCF);
	}
}
