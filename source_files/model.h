#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "my_vector.hpp"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	// mesh data
	gigachad::vector<struct Vertex>      vertices;
	gigachad::vector<unsigned int>		indices;
	gigachad::vector<struct Texture>     textures;

	Mesh() { }
	Mesh(gigachad::vector<struct Vertex> vertices, gigachad::vector<unsigned int> indices, gigachad::vector<struct Texture> textures);
	void Draw(GLuint shader);
private:
	//  render data
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

class Model
{
public:
	Model(const char* path)
	{
		loadModel(path);

		position = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
		rotation = glm::vec3(0.0f);
	}
	void Draw(GLuint shader, const glm::mat4& view, const glm::mat4& projection);

	static void ClearTextures() {
		for (int i = 0; i < textures_loaded.size(); i++)
			glDeleteTextures(1, &(textures_loaded[i].id));
		textures_loaded.clear();
	}

private:
	static gigachad::vector<struct Texture> textures_loaded;

	// model data
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;//degrees

	gigachad::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	gigachad::vector<struct Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);


public:
	void SetPosition(const glm::vec3& _position) { position = _position; }
	const glm::vec3& GetPosition() const { return position; }

	void SetScale(const glm::vec3& _scale) { scale = _scale; }
	const glm::vec3& GetScale() const { return scale; }

	void SetRotation(const glm::vec3& _rotation) { rotation = _rotation; }
	const glm::vec3& GetRotation() const { return rotation; }
};

#endif