#pragma once

#include "Utils.h"
#include "Geometry.h"
#include "Gameobject.h"

using namespace std;

class Level {

public:

	vector<Geometry*> levelObjects;

	Level();
	~Level();

	void init();

	void update();

	void draw();

	void reset();
	
	void addGeometry(Geometry* geometry);

	void addGameobject(Gameobject* gameObject);

private:

	vector<Gameobject*> gameObjects;
};