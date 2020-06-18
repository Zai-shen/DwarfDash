#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "Utils.h"
#include "DDSLoader.h"
#include "Shader.h"
#include "FPSCamera.h"

// CPU representation of a particle
struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	Particle() : pos(0.0f), speed(0.0f, 0.0f, 0.0f), life(-1.0f), cameradistance(-1.f) { }

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class ParticleGenerator
{
public:
	ParticleGenerator(std::shared_ptr<Shader> shader, FPSCamera* camera, int maxParticles = 1000, glm::vec3 position = glm::vec3(0.f));
	~ParticleGenerator();

	// update all particles
	void update(float dt);

	// render all particles
	void draw();

	FPSCamera* camera;

private:
	//position
	glm::vec3 position;

	// amount of particles
	int maxParticles;
	//std::vector<Particle> ParticlesContainer(10,particle);
	Particle ParticlesContainer[200];

	// render state
	std::shared_ptr<Shader> shader;
	GLuint VAO;
	GLuint Texture;
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	int ParticlesCount;

	// TBA
	GLfloat* g_particule_position_size_data = new GLfloat[maxParticles * 4];
	GLubyte* g_particule_color_data = new GLubyte[maxParticles * 4];

	// initializes buffer and vertex attributes
	void init();

	// returns the first Particle index that's currently unused
	int firstUnusedParticle();

	// sort particles
	void SortParticles();
};
