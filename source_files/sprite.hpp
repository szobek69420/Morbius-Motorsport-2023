#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>

class Sprite {
private:
	//non-static
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec2 scale;

	GLuint texture;
	glm::vec2 texturePosition;
	glm::vec2 textureScale;
	
	bool twoSided;

	//static
	static GLuint shader;
	static GLuint VAO, VBO, EBO;

public:
	//non-static
	Sprite();
	~Sprite() {};

	void SetPosition(const glm::vec3& _position) { position = _position; }
	void SetRotation(const glm::vec3& _rotation) { rotation = _rotation; }
	void SetScale(const glm::vec3& _scale) { scale = _scale; }
	void SetTexture(GLuint _texture) { texture = _texture; }
	void SetTexturePosition(const glm::vec2& _texturePosition) { texturePosition = _texturePosition; }
	void SetTextureScale(const glm::vec2& _textureScale) { textureScale = _textureScale; }
	void SetTwoSided(bool _twoSided) { twoSided = _twoSided; }

	glm::vec3& GetPosition() { return position; }
	glm::vec3& GetRotation() { return rotation; }
	glm::vec2& GetScale() { return scale; }
	GLuint GetTexture() { return texture; }
	glm::vec2& GetTexturePosition() { return texturePosition; }
	glm::vec2& GetTextureScale() { return textureScale; }
	bool isTwoSided() { return twoSided; }

	virtual void Render(const glm::mat4& view, const glm::mat4& projection);

	//static
	static void InitializeSpriteRenderer();
	static void UninitializeSpriteRenderer();
};

class Billboard :public Sprite {
private:
	glm::vec3 stareVictim;

public:
	Billboard();
	~Billboard() {}

	void SetStareVictim(const glm::vec3& _stareVictim);
	void RefreshRotation();

	virtual void Render(const glm::mat4& view, const glm::mat4& projection);

};

#endif