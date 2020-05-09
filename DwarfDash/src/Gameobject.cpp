#include "Gameobject.h"

using namespace std;

Gameobject::Gameobject() {}

Gameobject::Gameobject(Geometry* geometry) {
	this->goGeometry = geometry;
	cout << "Constructor with geometry called!" << endl;
}

Gameobject::~Gameobject() {}

void Gameobject::init() {
}

void Gameobject::update() {
	cout << "Updating gameobject!" << endl;
	PxMat44 transform;
	if (this->goActor)
	{
		cout << "Its a PhysX Actor!" << endl;
		transform = this->goActor->getGlobalPose();
	}else if (this->goDynamicActor) {
		cout << "Its a PhysX Dynamic Actor!" << endl;
		transform = this->goDynamicActor->getGlobalPose();
	}else {
		cout << "Error - no PhysX Actor!" << endl;
		return;
	}
	
	PxVec4 c1 = transform.column0;
	PxVec4 c2 = transform.column1;
	PxVec4 c3 = transform.column2;
	PxVec4 c4 = transform.column3;

	glm::vec4 v1 = glm::vec4(c1.x, c1.y, c1.z, c1.w);
	glm::vec4 v2 = glm::vec4(c2.x, c2.y, c2.z, c2.w);
	glm::vec4 v3 = glm::vec4(c3.x, c3.y, c3.z, c3.w);
	glm::vec4 v4 = glm::vec4(c4.x, c4.y, c4.z, c4.w);

	glm::mat4 glmTransform = glm::mat4(v1, v2, v3, v4);
	this->goGeometry->setModelMatrix(glmTransform);
}

void Gameobject::setAsDynamicActor(PxPhysics* gPhysics) {
	//PxBoxGeometry tempGeometry(PxVec3(0.5f, 0.5f, 0.5f)); //this->model
	//PxRigidDynamic* dunno;
	//dunno = PxCreateDynamic(gPhysics, this->goPosition, tempGeometry, this->goMaterial, 1.0f);
	//this->goDynamicActor = dunno;
}


void Gameobject::draw() {
	cout << "drawing gameobject!" << endl;
	this->goGeometry->draw();
}

void Gameobject::reset() {
}