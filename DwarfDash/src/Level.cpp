#include "Level.h"

using namespace std;

Level::Level() {}

Level::~Level() {}

void Level::init() {

}

void Level::update() {
	this->levelObjects[2]->transform(glm::rotate(glm::mat4(1.0f), glm::radians(.5f), glm::vec3(0.f, 1.f, 0.f)));
}

void Level::draw() {
	// Using a for loop with index
	for (std::size_t i = 0; i < this->levelObjects.size(); ++i) {
		this->levelObjects[i]->draw();
	}
}

void Level::reset() {
	this->levelObjects.clear();
}

void Level::addGeometry(Geometry* geometry) {
	this->levelObjects.push_back(geometry);
}