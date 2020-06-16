#pragma once

//With help from: https://learnopengl.com/particles

#include "Utils.h"
#include "Shader.h"

// Represents a single particle and its state
struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float     Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleSystem
{
public:
	ParticleSystem(std::shared_ptr<Shader> shader, unsigned int amount);
	
	// update all particles
	void Update(float dt, glm::vec2 position, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	
	// render all particles
	void Draw();
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
	void respawnParticle(Particle &particle, glm::vec2 pos, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};
