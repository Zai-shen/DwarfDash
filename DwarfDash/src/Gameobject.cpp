#include "Gameobject.h"

using namespace std;
using namespace physx;


Gameobject::Gameobject() {}

Gameobject::Gameobject(Geometry* geometry) {
	this->goGeometry = geometry;
}

Gameobject::Gameobject(Model* model) {
	this->goModel = model;
}

Gameobject::~Gameobject() {
	cout << "destroying gameobject variables" << endl;
	if (this->goActor) {
		goActor->release();
	}
	else if (this->goDynamicActor) {
		goDynamicActor->release();
	}
}

void Gameobject::init() {
}

void Gameobject::update() {
	PxMat44 transform;
	if (this->goActor)	{
		transform = this->goActor->getGlobalPose();
	}else if (this->goDynamicActor) {
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
	if (goGeometry)
	{
		this->goGeometry->setModelMatrix(glmTransform);
	}
	else if (goModel) {
		this->goModel->_shader->use();
		this->goModel->setModelMatrix(glmTransform);
		//this->goModel->_shader->setUniform("modelMatrix", this->goModel->getModelMatrix()); // this should be called right before the draw call 
	}
}

void Gameobject::draw() {
	if (goGeometry)
	{
		this->goGeometry->draw();
	}
	else if (goModel) {
		this->goModel->_shader->setUniform("modelMatrix", this->goModel->getModelMatrix());
		this->goModel->_shader->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(this->goModel->getModelMatrix()))));
		this->goModel->_shader->setUniform("texture_diffuse1", 0);
		this->goModel->draw();	
	}
}

void Gameobject::reset() {
}