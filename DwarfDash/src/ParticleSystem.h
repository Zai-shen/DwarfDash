#pragma once

//With help from: https://learnopengl.com/particles

#include "Utils.h"
#include "Shader.h"

// Represents a single particle and its state
struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	float     Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.f), Life(0.0f) { }
};

class ParticleSystem
{
public:
	ParticleSystem(std::shared_ptr<Shader> shader, unsigned int amount);
	
	// update all particles
	void Update(float dt, glm::vec3 position, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	
	// render all particles
	void ParticleSystem::Draw(glm::mat4 viewProjM, glm::vec3 camPosV);
	//void Draw();
private:
	// state
	std::vector<Particle> particles;
	unsigned int amount;

	// render state
	std::shared_ptr<Shader> shader;
	unsigned int VAO;

	// initializes buffer and vertex attributes
	void init();

	// returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	unsigned int firstUnusedParticle();

	// respawns particle
	void respawnParticle(Particle &particle, glm::vec3 pos, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};
