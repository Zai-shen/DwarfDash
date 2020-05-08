#pragma once

#include "Utils.h"
#include "Geometry.h"

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

private:

	int in = 2;
};