#include "Player.h"

using namespace std;
using namespace physx;


Player::Player() {}


Player::Player(PxPhysics* gPhysics, PxScene* gScene) {
	this->gPhysics = gPhysics;
	this->gScene = gScene;
	init();
}

Player::~Player() {
	cout << "destroying player variables" << endl;
	gPlayerController->release();
	gCCTManager->release();
}

void Player::init() {
	//Creating Character Controller Manager
	gCCTManager = PxCreateControllerManager(*gScene);

	//gCCTManager->setOverlapRecoveryModule(true);

	//Character Controller for Player (of type capsule)
	PxCapsuleControllerDesc charDesc;
	charDesc.height = pHeight; //height
	charDesc.radius = pRadius; //from height to top/bottom
	charDesc.position = pStartPos; //starting position
	charDesc.contactOffset = 0.00001f; //controller skin width for collisions
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
	collFlags = gPlayerController->move(PxVec3(displacement.x, displacement.y, displacement.z ), 0.001f, deltaTime, filter, nullptr);
}

void Player::wantsToJump(float deltaTime) {
	if (!jumping)
	{
		moveChar(glm::vec3(0.0, -0.1, 0.0), deltaTime); // moves down so collflags tracks a collision, and collision down gets forced
		if (collFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			currentHeight = gPlayerController->getPosition().y;
			cappedHeight = currentHeight + 5.f;
			jumping = true;
			jumpingUp = true;
			//cout << "collision down!" << endl;
		}
	}
}


void Player::jump(float deltaTime) {

	const PxControllerFilters pxCF;

	if (jumping)
	{
		currentHeight = gPlayerController->getPosition().y;
		if (currentHeight >= cappedHeight)
		{
			jumpingUp = false;
		}

		if (jumpingUp)
		{
			//JUMP
			moveChar(glm::vec3(0.0, -gGravity * deltaTime, 0.0), deltaTime, pxCF);
		}
		else
		{
			//FALL
			moveChar(glm::vec3(0.0, gGravity * deltaTime, 0.0), deltaTime, pxCF);
		}
		
		if (collFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			jumping = false;
		}
		if (collFlags & PxControllerCollisionFlag::eCOLLISION_UP)
		{
			jumping = false;
			//cout << "collision up!" << endl;
		}
	}
	else {
		//FALL
		moveChar(glm::vec3(0.0, gGravity * deltaTime, 0.0), deltaTime, pxCF);
	}
}
