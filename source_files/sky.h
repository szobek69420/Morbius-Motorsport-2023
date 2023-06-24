#ifndef SKY_H
#define SKY_H

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "my_vector.hpp"

//egy dolog (csillag, nap vagy barmi) az egben
//a tavolsag a jatekostol fix
class SkyElement {
private:
	float size;
	float rotation_x;//fok
	float rotation_y;//fok
	GLuint texture;

public:
	SkyElement(float size=1, float rotation_x=0, float rotation_y=0, GLuint texture_in=0)
	{
		this->size = size;
		this->rotation_x = rotation_x;
		this->rotation_y = rotation_y;

		//egy mar importalt texturat var
		texture = texture_in;
		//printf("%s\n", texturePath);
	}

	SkyElement(const SkyElement& theOther) : size(theOther.size), rotation_x(theOther.rotation_x), rotation_y(theOther.rotation_y), texture(theOther.texture) {}

	~SkyElement() { texture = 0; }

	float GetSize() { return size; }
	float GetRotationX() { return rotation_x; }
	float GetRotationY() { return rotation_y; }
	GLuint GetTexture() { return texture; }
};

//SkyElementeket kezelo osztaly
class Sky {
private:
	//render valtozok
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int skyshader;

	gigachad::vector<SkyElement> elements;

public:
	Sky()
	{
		//loading graphics data
		float vertices[] = {
		100.0,-5.0,-5.0,	0.0,0.0,
		100.0,5.0,-5.0,		0.0,1.0,
		100.0,5.0,5.0,		1.0,1.0,
		100.0,-5.0,5.0,		1.0,0.0
		};
		unsigned int indices[] = { 0,2,1,0,3,2 };

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));//texture
		glEnableVertexAttribArray(1);

		//compiling shader
		skyshader = createShader(SHADER_SKY);
		glUseProgram(skyshader);
		glUniform1i(glGetUniformLocation(skyshader, "skyTexture"), 0);
		glUseProgram(0);

		//andere
		elements.clear();
	}

	//kitorol minden maradvanyt a memoriakbol
	~Sky()
	{
		elements.clear();

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteProgram(skyshader);
	}

	//operatortulterheles egy elegansabb hozzaadashoz
	Sky& operator+=(const SkyElement& skyElement)
	{
		this->elements.push_back(skyElement);
		return (*this);
	}

	//megjelenito fuggveny
	void RenderSky(const glm::mat4& pv, const glm::vec3& camPos)
	{
		glm::mat4 model = glm::mat4(1.0);

		glDisable(GL_CULL_FACE);
		glBindVertexArray(vao);

		//printf("element[0].texture: %u\n", elements[0].GetTexture());
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, elements[0].GetTexture());

		glUseProgram(skyshader);
		glUniform4f(glGetUniformLocation(skyshader, "camPos"), camPos.x, camPos.y, camPos.z, 0.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyshader, "pv"), 1, GL_FALSE, glm::value_ptr(pv));


		int length = elements.size();
		for (int i = 0; i < length; i++)
		{
			model = glm::mat4(1.0);
			model = glm::rotate(model, glm::radians(elements[i].GetRotationY()), glm::vec3(0.0, 1.0, 0.0));
			model = glm::rotate(model, glm::radians(elements[i].GetRotationX()), glm::vec3(0.0, 0.0, 1.0));

			glUniformMatrix4fv(glGetUniformLocation(skyshader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniform1f(glGetUniformLocation(skyshader, "size"), elements[i].GetSize());

			glBindTexture(GL_TEXTURE_2D, elements[i].GetTexture());

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		}

		glEnable(GL_CULL_FACE);
	}
};

#endif