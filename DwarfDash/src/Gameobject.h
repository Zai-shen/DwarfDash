#pragma once

#include <PxPhysicsAPI.h>
#include "Geometry.h"
//#include "Model.h"

using namespace std;
using namespace physx;

class Gameobject {

public:
	PxMaterial* goMaterial;
	PxTransform goPosition;
	PxRigidStatic* goActor;
	PxRigidDynamic* goDynamicActor;
	Geometry* goGeometry;
	//Model* goModel;

	Gameobject();

	Gameobject(Geometry* geometry);

	//Gameobject(Model* model);

	~Gameobject();

	void init();

	void update();

	void draw();

	void reset();

	void setAsDynamicActor(PxPhysics* gPhysics);

private:

};