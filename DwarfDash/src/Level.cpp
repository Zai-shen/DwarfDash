#include "Level.h"

using namespace std;

Level::Level() {}

Level::~Level() {}

void Level::init() {

}

void Level::update() {
	cout << "updating level!" << endl;
	// Using a for loop with index
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->update();
	}

	//this->levelObjects[2]->transform(glm::rotate(glm::mat4(1.0f), glm::radians(.5f), glm::vec3(0.f, 1.f, 0.f)));
}

void Level::draw() {
	cout << "drawing level!" << endl;
	//// Using a for loop with index
	//for (std::size_t i = 0; i < this->levelObjects.size(); ++i) {
	//	this->levelObjects[i]->draw();
	//}

	cout << gameObjects.size() << endl;
	// Using a for loop with index
	for (std::size_t i = 0; i < this->gameObjects.size(); ++i) {
		this->gameObjects[i]->draw();
	}
}

void Level::reset() {
	this->levelObjects.clear();
}

void Level::addGeometry(Geometry* geometry) {
	this->levelObjects.push_back(geometry);
}

void Level::addGameobject(Gameobject* gameObject) {
	this->gameObjects.push_back(gameObject);
}
