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
}

void FlareRenderer::draw() {
	for (std::size_t i = 0; i < flares.size(); ++i) {
		flares[i]->draw();
	}
}

void FlareRenderer::init() {
	glm::vec3 sunOrigin = glm::vec3(12.f, 80.f, -60.f);
	glm::vec3 sunToPlayer = player->getPosition() - sunOrigin;
    glm::vec3 positionExample = sunOrigin + (sunToPlayer * 0.9f);
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/sun.png", this->camera, this->player, glm::vec2(100.f, 100.f), sunOrigin, 0.001f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex9.png", this->camera, this->player, glm::vec2(5.f, 5.f), sunOrigin, 0.1f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex8.png", this->camera, this->player, glm::vec2(30.f, 30.f), sunOrigin, 0.15f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex7.png", this->camera, this->player, glm::vec2(3.f, 3.f), sunOrigin, 0.3f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex6.png", this->camera, this->player, glm::vec2(20.f, 20.f), sunOrigin, 0.33f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex5.png", this->camera, this->player, glm::vec2(2.f, 2.f), sunOrigin, 0.55f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex4.png", this->camera, this->player, glm::vec2(10.f, 10.f), sunOrigin, 0.6f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex3.png", this->camera, this->player, glm::vec2(1.5f, 1.5f), sunOrigin, 0.66f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex2.png", this->camera, this->player, glm::vec2(1.f, 1.f), sunOrigin, 0.8f));
	flares.push_back(new FlareTexture(this->shader, "assets/textures/lensFlare/tex1.png", this->camera, this->player, glm::vec2(0.1f, 0.1f), sunOrigin, 0.87f));
}