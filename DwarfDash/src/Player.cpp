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

	//Character Controller for Player (of type capsule)
	PxCapsuleControllerDesc charDesc;
	charDesc.position = PxExtendedVec3(-3.0f, 3.0f, 0.0f); //starting position
	charDesc.height = PxF32(0.8f); //height
	charDesc.radius = PxF32(0.1f); //from height to top/bottom
	charDesc.contactOffset = 0.05f; //controller skin width for collisions
	charDesc.stepOffset = 0.01; //max obstacle climb height
	charDesc.slopeLimit = cosf(glm::radians(45.0f)); // max slope to walk
	charDesc.upDirection = PxVec3(0, 1, 0); // Specifies the 'up' direction
	charDesc.material = gPhysics->createMaterial(0.1f, 0.1f, 0.1f);

	gPlayerController = gCCTManager->createController(charDesc);
}

void Player::update() {
	gPlayerController->move(PxVec3(0.001f, 0.0f, 0.0f), 0.0005, 1/60, nullptr, nullptr); // 1/60 should be dt?
}

void Player::draw() {
	cout << "Controller pos:" << endl;
	cout << gPlayerController->getPosition().x << "x " << gPlayerController->getPosition().y << "y " << gPlayerController->getPosition().z << "z " << endl;
}

void Player::reset() {

}
