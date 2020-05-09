#include "Level.h"

using namespace std;

Level::Level() {}

Level::~Level() {}

void Level::init() {
}

void Level::update() {
	// Using a for loop with index
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->update();
	}
}

void Level::draw() {
	// Using a for loop with index
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->draw();
	}
}

void Level::reset() {
	this->gameObjects.clear();
}

void Level::addGameobject(Gameobject* gameObject) {
	this->gameObjects.push_back(gameObject);
}
