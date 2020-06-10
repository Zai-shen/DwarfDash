#pragma once

#include <PxPhysicsAPI.h>
#include "Geometry.h"
#include "Model.h"

using namespace std;
using namespace physx;

class Gameobject {

public:
	PxMaterial* goMaterial;
	PxTransform goPosition;
	PxRigidStatic* goActor;
	PxRigidDynamic* goDynamicActor;
	Geometry* goGeometry;
	Model* goModel;

	Gameobject();

	Gameobject(Geometry* geometry);

	Gameobject(Model* model);

	~Gameobject();

	void init();

	void update(float dt);

	void draw();

	void reset();

private:

	PxVec3 kineMoveDir = PxVec3(-2.f,0.f,0.f);
};