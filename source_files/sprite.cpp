#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "sprite.hpp"
#include "shader.h"

//class Sprite
Sprite::Sprite()
{
	position = glm::vec3(0.0);
	rotation = glm::vec3(0.0);
	scale = glm::vec2(1.0);

	texture = 0;

	texturePosition = glm::vec2(0.0);
	textureScale = glm::vec2(1.0);

	twoSided = true;
}

void Sprite::Render(const glm::mat4& view, const glm::mat4& projection)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (twoSided)
		glDisable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
	//model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));

	glm::mat4 textureMatrix = glm::mat4(1.0);
	textureMatrix = glm::translate(textureMatrix, glm::vec3(texturePosition.x, texturePosition.y, 0.0));
	textureMatrix = glm::scale(textureMatrix, glm::vec3(textureScale.x, textureScale.y, 1.0));


	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "textureMatrix"), 1, GL_FALSE, glm::value_ptr(textureMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(0);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);
}


//class Sprite static

GLuint Sprite::shader = 0, Sprite::VAO = 0, Sprite::VBO = 0, Sprite::EBO = 0;

void Sprite::InitializeSpriteRenderer()
{
	shader = createShader(SHADER_SPRITE);
	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "sprite"), 0);
	glUseProgram(0);

	//buffer setup
	float vertices[] = {
		-0.5,-0.5,0.0,0.0,0.0,
		-0.5,0.5,0.0,0.0,1.0,
		0.5,0.5,0.0,1.0,1.0,
		0.5,-0.5,0.0,1.0,0.0
	};
	unsigned int indices[] = { 0,1,2,0,2,3 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (const void*)vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (const void*)indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Sprite::UninitializeSpriteRenderer()
{
	if (shader != 0)
		glDeleteProgram(shader);
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);
	if (VBO != 0)
		glDeleteBuffers(1, &VBO);
	if (EBO != 0)
		glDeleteBuffers(1, &EBO);

	shader = 0;
	VAO = 0;
	VBO = 0;
	EBO = 0;
}

//class Billboard
Billboard::Billboard() :Sprite()
{
	stareVictim = glm::vec3(0.0);
}

void Billboard::SetStareVictim(const glm::vec3& _stareVictim)
{
	stareVictim = _stareVictim;

	RefreshRotation();
	//std::cout << "X: " << _rotation.x << ", Y: " << _rotation.y << ", Z: " << _rotation.z << std::endl;
}

void Billboard::RefreshRotation()
{
	//calculating rotation
	glm::vec3 _delta = stareVictim - this->GetPosition();
	_delta = glm::normalize(_delta);

	glm::vec3 _rotation = glm::vec3(0.0);
	_rotation.x = -glm::asin(_delta.y);
	_delta.y = 0.0;
	_delta = glm::normalize(_delta);
	_rotation.y = glm::atan(_delta.x / _delta.z);
	if (_delta.z < 0.0) _rotation.y += glm::pi<float>();
	_rotation.z = 0;

	_rotation *= (180.0f / glm::pi<float>());

	this->SetRotation(_rotation);
}

void Billboard::Render(const glm::mat4& view, const glm::mat4& projection)
{
	//render
	this->Sprite::Render(view, projection);
}