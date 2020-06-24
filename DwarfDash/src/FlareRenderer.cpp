#include "FlareRenderer.h"


FlareRenderer::FlareRenderer(std::shared_ptr<Shader> shader, FPSCamera* camera, Player* player)
	:camera(camera), player(player){
	this->shader = shader;
	this->init();
}

FlareRenderer::~FlareRenderer() {

}

void FlareRenderer::update(float dt) {
	for (std::size_t i = 0; i < flares.size(); ++i) {
		flares[i]->update(dt);
	}
	//std::cout << player->getPosition().z << std::endl;
}

void FlareRenderer::draw() {
	for (std::size_t i = 0; i < flares.size(); ++i) {
		flares[i]->draw();
	}
}

void FlareRenderer::init() {
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/sun.png", this->camera, glm::vec2(100.f, 100.f), glm::vec3(12.f, 80.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex9.png", this->camera, glm::vec2(10.f, 10.f), glm::vec3(12.f, 78.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex8.png", this->camera, glm::vec2(60.f, 60.f), glm::vec3(12.f, 76.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex7.png", this->camera, glm::vec2(10.f, 10.f), glm::vec3(12.f, 72.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex6.png", this->camera, glm::vec2(50.f, 50.f), glm::vec3(12.f, 68.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex5.png", this->camera, glm::vec2(10.f, 10.f), glm::vec3(12.f, 64.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex4.png", this->camera, glm::vec2(40.f, 40.f), glm::vec3(12.f, 60.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex3.png", this->camera, glm::vec2(10.f, 10.f), glm::vec3(12.f, 56.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex2.png", this->camera, glm::vec2(20.f, 20.f), glm::vec3(12.f, 52.f, -60.f)));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex1.png", this->camera, glm::vec2(1.f, 1.f), glm::vec3(12.f, 48.f, -60.f)));
}