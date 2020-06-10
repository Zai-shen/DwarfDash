/*
This code is based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h
*/

#pragma once

#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h" // ecg


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct MeshTexture {
	unsigned int id;
	string type;
	string path;
};

struct MeshMaterial {
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<MeshTexture> textures;
	MeshMaterial material;

	float ambient;
	float diffuse;
	float specular;


	//Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<MeshTexture> textures){
	Mesh(vector<Vertex> &vertices, vector<unsigned int> &indices, vector<MeshTexture> &textures, MeshMaterial material){

		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->material = material;

		this->ambient = 0.4f;
		this->diffuse = 1.0f;
		this->specular = 0.8f;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	// render the mesh
	void draw(Shader &shader)	{

		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // actually Height texture according to tuwel
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // actually Ambient  texture according to tuwel

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.getHandle(), (name + number).c_str()), i);

			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glUniform3f(glGetUniformLocation(shader.getHandle(), "materialCoefficients"), ambient, diffuse, specular);
		glUniform1f(glGetUniformLocation(shader.getHandle(), "specularAlpha"), 10.0f);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	// to set the values to something different than the initial values
	void setMaterialCoefficients(float ambient, float diffuse, float specular) {
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;

		//std::cout << ambient << std::endl;
	}

private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()	{

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		// unbind
		glBindVertexArray(0);

		// TODO: set texture parameters (mag_filter, min_filter ...)
	}
};
#endif
