#include "Level.h"

using namespace std;

Level::Level() {}

Level::~Level() {
	//cout << "destroying level variables" << endl;
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->~Gameobject();
	}
	//this->gameObjects.clear();
}

void Level::init() {
}

void Level::update(float dt) {
	// Using a for loop with index
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->update(dt);
	}
}

void Level::draw() {
	// Using a for loop with index
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->draw();
	}
}

void Level::reset() {
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
	//	this->gameObjects[i]->~Gameobject();
		this->gameObjects[i]->reset();
	}
	//this->gameObjects.clear();
}

void Level::addGameobject(Gameobject* gameObject) {
	this->gameObjects.push_back(gameObject);
}
