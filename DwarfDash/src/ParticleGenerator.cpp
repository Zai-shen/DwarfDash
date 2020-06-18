#include "ParticleGenerator.h"
#include <glm/gtx/string_cast.hpp>
#include <random>

using namespace glm;

//with help from: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/


ParticleGenerator::ParticleGenerator(std::shared_ptr<Shader> shader, FPSCamera* camera, int maxParticles, glm::vec3 position)
	: maxParticles(maxParticles), position(position), camera(camera)
{
	this->shader = shader;
	this->init();
}


ParticleGenerator::~ParticleGenerator()
{
	delete[] g_particule_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(shader->getHandle());
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VAO);
}




int LastUsedParticle = 0;
// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int ParticleGenerator::firstUnusedParticle() {

	for (int i = LastUsedParticle; i < maxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void ParticleGenerator::SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[maxParticles]);
}

void ParticleGenerator::init() {
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create this->maxParticles default particle instances
	/*for (int i = 0; i < this->maxParticles; i++) {
		//std::cout << i << " of " << maxParticles << std::endl;
		this->ParticlesContainer.push_back(Particle());
	}*/
	
	for (int i = 0; i < maxParticles; i++) {
		Particle& p = ParticlesContainer[i];
		p.life = -1.0f;
		p.cameradistance = -1.0f;
	}
	
	//Texture = loadDDSint("particle.DDS");
	Texture = loadDDSint("assets/textures/particle.DDS");

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);


}

void ParticleGenerator::update(float dt) {

	
	//glm::mat4 ProjectionMatrix = camera->getProjectionMatrix();
	//glm::mat4 ViewMatrix = camera->getViewMatrix();

	// We will need the camera's position in order to sort the particles
	// w.r.t the camera's distance.
	// There should be a getCameraPosition() function in common/controls.cpp, 
	// but this works too.
	//glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
	glm::vec3 CameraPosition = camera->getPosition();

	//glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glm::mat4 ViewProjectionMatrix = camera->getViewProjectionMatrix();

	// Generate 5 new particule each millisecond,
	// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
	// newparticles will be huge and the next frame even longer.
	int newparticles = (int)(dt*5000.0);
	if (newparticles > (int)(0.016f*5000.0))
		newparticles = (int)(0.016f*5000.0);

	///std::cout << "new particles: " << newparticles << std::endl;

	for (int i = 0; i < newparticles; i++) {
		int particleIndex = firstUnusedParticle();
		Particle& p = ParticlesContainer[particleIndex]; // shortcut
		float randPointFive = (rand() % 100)/200.f;
		p.life = 1.0f + randPointFive; // 1.f : 1 second
		p.pos = this->position;

		float spread = 1.0f;
		glm::vec3 maindir = glm::vec3(0.0f, 3.0f, 0.0f);
		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		p.speed = maindir + randomdir * spread;


		// Very bad way to generate a random color
		p.r = 0;// rand() % 256;
		p.g = 0;//rand() % 256;
		p.b = rand() % 128;
		p.a = (rand() % 256) / 3;

		p.size = (rand() % 1000) / 6000.0f + 0.001f;
	}


	// Update all particles
	ParticlesCount = 0;
	for (int i = 0; i < maxParticles; i++) {

		Particle& p = ParticlesContainer[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= dt;
			if (p.life > 0.0f) {

				if (p.b > 0)
				{
					p.b -= 300 * dt;
				}
				if (p.r < 256)
				{
					p.r += 400 * dt;
				}
				///std::cout << "particle number: " << i << " with position: " << to_string(p.pos) << std::endl;

				// Simulate simple physics : gravity only, no collisions
				p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)dt * 0.25f;
				p.pos += p.speed * (float)dt;
				p.cameradistance = glm::length2(p.pos - CameraPosition);

				// Fill the GPU buffer
				g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
				g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
				g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

				g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

				g_particule_color_data[4 * ParticlesCount + 0] = p.r;
				g_particule_color_data[4 * ParticlesCount + 1] = p.g;
				g_particule_color_data[4 * ParticlesCount + 2] = p.b;
				g_particule_color_data[4 * ParticlesCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}
			ParticlesCount++;
		}
	}
	SortParticles();
}

void ParticleGenerator::draw() {
	//Bind VAO
	glBindVertexArray(this->VAO);

	// Update the buffers that OpenGL uses for rendering.
	// There are much more sophisticated means to stream data from the CPU to the GPU.
	// http://www.opengl.org/wiki/Buffer_Object_Streaming
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);

	// Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	shader->use();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our textureSampler sampler to use Texture Unit 0
	shader->setUniform("textureSampler", 0);

	// Same as the billboards tutorial
	shader->setUniform("CameraRight_worldspace", glm::vec3(camera->getViewMatrix()[0][0], camera->getViewMatrix()[1][0], camera->getViewMatrix()[2][0]));
	shader->setUniform("CameraUp_worldspace", glm::vec3(camera->getViewMatrix()[0][1], camera->getViewMatrix()[1][1], camera->getViewMatrix()[2][1]));
	shader->setUniform("VP", camera->getViewProjectionMatrix());

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : x + y + z + size => 4
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE,                 // type
		GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	//Unbind VAO
	glBindVertexArray(0);
}

