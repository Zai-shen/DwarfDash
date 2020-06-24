#include "FlareRenderer.h"


FlareRenderer::FlareRenderer(std::shared_ptr<Shader> shader, FPSCamera* camera)
	:camera(camera){
	this->shader = shader;
	this->init();
}

FlareRenderer::~FlareRenderer() {

}

void FlareRenderer::update(float dt) {
	for (std::size_t i = 0; i < flares.size(); ++i) {
		flares[i]->update(dt);
	}
}

void FlareRenderer::draw() {
	for (std::size_t i = 0; i < flares.size(); ++i) {
		flares[i]->draw();
	}
}

void FlareRenderer::init() {
	flares.push_back(new FlareTexture(this->shader, "assets/textures/particle.DDS", this->camera, 1.f, glm::vec2(0.5f,0.5f)));
	//flares.push_back(new FlareTexture());
	//flares.push_back(new FlareTexture());
	//flares.push_back(new FlareTexture());

}