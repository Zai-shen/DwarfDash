/*
Code based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
*/
#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

class Model
{
public:
	vector<MeshTexture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	std::shared_ptr<Shader> _shader; // pointer that is used for the model
	glm::mat4 _modelMatrix = glm::mat4(1.0f);
	glm::mat4 _transformMatrix;

	Model();

	Model(string const& path, std::shared_ptr<Shader> shader);
	Model(string const& path, std::shared_ptr<Shader> shader, glm::mat4 modelMatrix);

	// draws the model, and thus all its meshes
	void draw();

	glm::mat4 getModelMatrix();

	void setTransformMatrix(glm::mat4 transformMatrix);

	void transform(glm::mat4 transformMatrix);

private:
	void loadModel(string const& path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	MeshMaterial loadMaterial(aiMaterial* mat);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

	unsigned int TextureFromFile(const char* path, const string& directory);


};


#endif