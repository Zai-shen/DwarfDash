#include "ParticleSystem.h"
#include <glm/gtx/string_cast.hpp>

ParticleSystem::ParticleSystem(std::shared_ptr<Shader> shader, unsigned int amount)
	: amount(amount)
{
	this->shader = shader;
	this->init();
}

void ParticleSystem::Update(float dt, glm::vec3 position, unsigned int newParticles, glm::vec3 offset)
{
	// add new particles 
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], position, offset);
	}

	// update all particles
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position += p.Velocity * dt;
			p.Color.a -= dt * 2.5f;
			//std::cout << glm::to_string(p.Position) << " <pos color> " << glm::to_string(p.Color) << std::endl;
		}
	}
}

// render all particles
void ParticleSystem::Draw(glm::mat4 viewProjM, glm::vec3 camPosV)
{
	// use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader->use();
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader->setUniform("viewProjMatrix", viewProjM);
			//this->shader->setUniform("camera_world", camera.getPosition());
			this->shader->setUniform("offset", particle.Position);
			this->shader->setUniform("color", particle.Color);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::init()
{
	// set up mesh and attribute properties
	unsigned int VBO;
	float particle_quad[] = {
		0.0f, 0.5f, 0.0f, 0.5f,
		0.5f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f, 0.5f,
		0.5f, 0.5f, 0.5f, 0.5f,
		0.5f, 0.0f, 0.5f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// create this->amount default particle instances
	for (unsigned int i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleSystem::firstUnusedParticle()
{
	// first search from last used particle, this will usually return almost instantly
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// otherwise, do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleSystem::respawnParticle(Particle &particle, glm::vec3 pos, glm::vec3 offset)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = pos + random + offset;
	//particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Color = glm::vec4(rColor, 0.f, 0.f, 1.0f);
	particle.Life = 5.0f;
	//particle.Velocity = glm::vec3(0.1f,1.f,-0.1f);
}