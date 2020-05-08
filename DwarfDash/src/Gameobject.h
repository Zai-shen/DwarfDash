#pragma once

#include <PxPhysicsAPI.h>
#include "Geometry.h"

using namespace std;
using namespace physx;

class Gameobject {

public:

	Gameobject();
	~Gameobject();

	void init();

	void update();

	void draw();

	void reset();



private:

	PxMaterial* goMaterial;
	PxTransform goPosition;
	PxRigidStatic* goActor;
	PxRigidDynamic* goDynamicActor;
	Geometry* goGeometry;

};