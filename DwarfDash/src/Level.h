#pragma once

#include "Utils.h"
#include "Geometry.h"
#include "Gameobject.h"

using namespace std;

class Level {

public:

	Level();
	~Level();

	void init();

	void update();

	void draw();

	void reset();

	void addGameobject(Gameobject* gameObject);

private:

	vector<Gameobject*> gameObjects;
};