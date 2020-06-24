//with help from: https://www.youtube.com/channel/UCUkRj4qoT1bsWpE_C8lZYoQ

#include "FlareTexture.h"

FlareTexture::FlareTexture(){}

FlareTexture::FlareTexture(std::shared_ptr<Shader> shader, string texture, FPSCamera* camera, glm::vec2 scaleTex, glm::vec3 pos)
	:camera(camera){
	this->shader = shader;
	this->position = pos;
	this->scaleTexture = scaleTex;
	this->init(texture);
}

FlareTexture::~FlareTexture() {

}

void FlareTexture::init(string texture) {
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	int width, height, nrChannels;

	unsigned char *data = stbi_load(texture.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Flare texture failed to load at path: " << texture << std::endl;
	}
	stbi_image_free(data);

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

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// update all particles
void FlareTexture::update(float dt) {

}

// render all particles
void FlareTexture::draw() {
	//Bind VAO
	glBindVertexArray(this->VAO);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	shader->use();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, texId);
	// Set our textureSampler sampler to use Texture Unit 0
	shader->setUniform("texSampler", 7);

	glm::mat4 VM = camera->getViewMatrix();
	// Same as the billboards tutorial
	shader->setUniform("CameraRight_worldspace", glm::vec3(VM[0][0], VM[1][0], VM[2][0]));
	shader->setUniform("CameraUp_worldspace", glm::vec3(VM[0][1], VM[1][1], VM[2][1]));
	shader->setUniform("BillboardPos", position);
	shader->setUniform("BillboardSize", scaleTexture);
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

