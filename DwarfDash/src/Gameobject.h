#pragma once

#include <PxPhysicsAPI.h>
#include "Geometry.h"

using namespace std;
using namespace physx;

class Gameobject {

public:
	PxMaterial* goMaterial;
	PxTransform goPosition;
	PxRigidStatic* goActor;
	PxRigidDynamic* goDynamicActor;
	Geometry* goGeometry;

	Gameobject();

	Gameobject(Geometry* geometry);

	~Gameobject();

	void init();

	void update();

	void draw();

	void reset();

	void setAsDynamicActor(PxPhysics* gPhysics);

private:

};