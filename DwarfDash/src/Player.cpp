#include "Player.h"

using namespace std;
using namespace physx;

int Player::score = 0;
bool Player::hasWon = false;
bool Player::hasLost = false;

class UserControllerHitReport : public PxUserControllerHitReport
{
	void onShapeHit(const PxControllerShapeHit &hit)
	{
		//std::cout << "player hit a shape at position: " << hit.actor->getGlobalPose().p.x << " " << hit.actor->getGlobalPose().p.y << " " << hit.actor->getGlobalPose().p.z << std::endl;
		if (hit.actor->getName() == "coin" || hit.actor->getName() == "heart" || hit.actor->getName() == "shield")
		{
			// + score
			if (hit.actor->getName() == "coin")
			{
				Player::score += 1;
			}

			// release actor
			if (hit.actor->isReleasable()) {
				hit.actor->setGlobalPose(PxTransform(PxVec3(-150.f,0.f,150.f)),false);
				//hit.actor->release();
			}
		}

		if (hit.actor->getName() == "goal")
		{
			//set game to won / next level
			Player::hasWon = true;
		}

		if (hit.actor->getName() == "cloud" || hit.actor->getName() == "ground")
		{
			//set game to lost / first level
			Player::hasLost = true;
		}
	};

	void onControllerHit(const PxControllersHit &hit)
	{
		//std::cout << "player hit a controller" << std::endl;
	};

	void onObstacleHit(const PxControllerObstacleHit &hit)
	{
		//std::cout << "player hit an obstacle" << std::endl;
	};
};

UserControllerHitReport controllerHitReport;

Player::Player() {}

Player::Player(PxPhysics* gPhysics, PxScene* gScene) {
	this->gPhysics = gPhysics;
	this->gScene = gScene;
	init();
}

Player::~Player() {
	//cout << "destroying player variables" << endl;
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
	charDesc.reportCallback = &controllerHitReport;


	gPlayerController = gCCTManager->createController(charDesc);
}

void Player::setToStartPosition() {
	gPlayerController->setPosition(pStartPos);
}

void Player::update(float dt) {
}

void Player::draw() {
	//cout << "Controller pos:" << endl;
	//cout << gPlayerController->getPosition().x << "x " << gPlayerController->getPosition().y << "y " << gPlayerController->getPosition().z << "z " << endl;
}

void Player::reset() {
	score = 0;
	hasWon = false;
	hasLost = false;
	setToStartPosition();
}

glm::vec3 Player::getPosition() {
	return glm::vec3(gPlayerController->getPosition().x, gPlayerController->getPosition().y, gPlayerController->getPosition().z);
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
