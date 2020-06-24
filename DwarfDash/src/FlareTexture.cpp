//with help from: https://www.youtube.com/channel/UCUkRj4qoT1bsWpE_C8lZYoQ

#include "FlareTexture.h"

FlareTexture::FlareTexture()
	:texture(Texture("assets/textures/particle.DDS")){
	//standard texture
}

FlareTexture::FlareTexture(std::shared_ptr<Shader> shader, string texture, FPSCamera* camera, float scale, glm::vec2 screenPos)
	:texture(Texture(texture)), camera(camera){
	this->shader = shader;
	this->init();
}

FlareTexture::~FlareTexture() {

}

void FlareTexture::init() {
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// update all particles
void FlareTexture::update(float dt) {

}

// render all particles
void FlareTexture::draw() {
	//Bind VAO
	glBindVertexArray(this->VAO);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	shader->use();

	// Bind our texture in Texture Unit 0
	///glActiveTexture(GL_TEXTURE0);
	///glBindTexture(GL_TEXTURE_2D, Texture);
	texture.bind(GL_TEXTURE0);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	///glUniform1i(TextureID, 0);
	shader->setUniform("texSampler", 0);

	glm::mat4 VM = camera->getViewMatrix();
	// Same as the billboards tutorial
	shader->setUniform("CameraRight_worldspace", glm::vec3(VM[0][0], VM[1][0], VM[2][0]));
	shader->setUniform("CameraUp_worldspace", glm::vec3(VM[0][1], VM[1][1], VM[2][1]));
	shader->setUniform("BillboardPos",glm::vec3(0.0f,2.f,-2.f));
	shader->setUniform("BillboardSize", glm::vec2(1.f, 1.f));
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

	// This draws a triangle_strip which looks like a quad.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
}

