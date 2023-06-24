/*

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <vector>
#include <glad/glad.h>
#include "ui.h"
#include "shader.h"

UIElement::UIElement(const char* _name)
{
	type = PASSIVE;

	size[0] = 1.0f;
	size[1] = 1.0f;

	position[0] = 0.0f;
	position[1] = 0.0f;

	uvsize[0] = 1.0f;
	uvsize[1] = 1.0f;
	
	uvposition[0] = 0.0f;
	uvposition[1] = 0.0f;

	background_texture = 0;
	text_texture = 0;
	text_length = 0;
	text[0] = '\0';

	if (strlen(_name) == 0)
		strcpy(name, "element");
	else
		strcpy(name, _name);

	buttonCallback = NULL;

}

UIElement::UIElement(void (*_buttonCallback)(void), unsigned int _background_texture, const char* _text, const char* _name)
{
	type = BUTTON;

	size[0] = 1.0f;
	size[1] = 1.0f;

	position[0] = 0.0f;
	position[1] = 0.0f;

	uvsize[0] = 1.0f;
	uvsize[1] = 1.0f;

	uvposition[0] = 0.0f;
	uvposition[1] = 0.0f;

	background_texture = 0;
	text_texture = 0;
	text_length = 0;
	text[0] = '\0';

	if (strlen(_name) == 0)
		strcpy(name, "element");
	else
		strcpy(name, _name);

	buttonCallback = _buttonCallback;
}

UIElement::UIElement(enum uielement_type _type, float _uvsizex, float _uvsizey, float _uvpositionx, float _uvpositiony, unsigned int _background_texture, const char* _text, const char* _name)
{
	type = _type;

	size[0] = 1.0f;
	size[1] = 1.0f;

	position[0] = 0.0f;
	position[1] = 0.0f;


	uvsize[0] = _uvsizex;
	uvsize[1] = _uvsizey;

	uvposition[0] = _uvpositionx;
	uvposition[1] = _uvpositiony;

	background_texture = _background_texture;

	strcpy(text, _text);
	text_length = strlen(text);
	text_texture = 0;

	if (strlen(_name) == 0)
		strcpy(name, "element");
	else
		strcpy(name, _name);

	buttonCallback = NULL;
}

UIElement::~UIElement()
{
	if (text_texture != 0)
	{
		TextureImporter::DestroyTexture(text_texture);
		text_texture = 0;
	}
}

void UIElement::setPosition(float x, float y)
{
	position[0] = x;
	position[1] = y;
}

void UIElement::setSize(float x, float y)
{
	size[0] = x;
	size[1] = y;
}

void UIElement::setUVPosition(float x, float y)
{
	uvposition[0] = x;
	uvposition[1] = y;
}

void UIElement::setUVSize(float x, float y)
{
	uvsize[0] = x;
	uvsize[1] = y;
}

void UIElement::setBackground(unsigned int _background_texture)
{
	background_texture = _background_texture;
}

void UIElement::setCallback(void(*_buttonCallback)(void))
{
	buttonCallback = _buttonCallback;
}



//canvas
Canvas::Canvas(bool _visible)
{
	visible = _visible;
	uielements.clear();


	//buffer setup
	float vertices[] = {
		0.0,0.0,0.0,0.0,
		0.0,1.0,0.0,1.0,
		1.0,1.0,1.0,1.0,
		1.0,0.0,1.0,0.0
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

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);//inData
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//shadersetup

	shader = createShader(SHADER_UI);

	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "Texture"), 0);
	glUniform1f(glGetUniformLocation(shader, "dankness"), 1.0f);
	glUseProgram(0);
}

Canvas::~Canvas()
{
	for (int i = uielements.size()-1; i >=0; i--)
	{
		delete uielements[i];
	}
	uielements.clear();
	uielements.resize(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	destroyShader(shader);

	vao = 0;
	vbo = 0;
	ebo = 0;
	shader = 0;
}

void Canvas::AddElement(enum uielement_type _type, const char* _name, unsigned int _background_texture, const char* _text)
{
	UIElement* temp = new UIElement(_type, 1.0f, 1.0f, 0.0f, 0.0f, _background_texture, _text, _name);
	uielements.push_back(temp);
}

void Canvas::AddElement(const char* _name, void(*_buttonCallback)(void), unsigned int _background_texture, const char* _text)
{
	UIElement* temp = new UIElement(BUTTON, 1.0f, 1.0f, 0.0f, 0.0f, _background_texture, _text, _name);
	temp->setCallback(_buttonCallback);
	uielements.push_back(temp);
}

void Canvas::Render(float cursorX, float cursorY, bool leftClick)
{
	if (!visible) return;

	unsigned int length = uielements.size();

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shader);
	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	GLint location_pos = glGetUniformLocation(shader, "position");
	GLint location_uv = glGetUniformLocation(shader, "uvposition");
	for (int i = 0; i < length; i++)
	{
		//background
		glBindTexture(GL_TEXTURE_2D, uielements[i]->background_texture);
		glUniform4f(location_pos, uielements[i]->position[0], uielements[i]->position[1], uielements[i]->size[0], uielements[i]->size[1]);
		glUniform4f(location_uv, uielements[i]->uvposition[0], uielements[i]->uvposition[1], uielements[i]->uvsize[0], uielements[i]->uvsize[1]);

		if (uielements[i]->type == BUTTON)
		{
			if (uielements[i]->position[0]<cursorX && uielements[i]->position[0] + uielements[i]->size[0]>cursorX&& uielements[i]->position[1]<cursorY&& uielements[i]->position[1] + uielements[i]->size[1]>cursorY)
			{
				glUniform1f(glGetUniformLocation(shader, "dankness"), 0.8f);
				if (leftClick)
					uielements[i]->buttonCallback();
			}
			else
				glUniform1f(glGetUniformLocation(shader, "dankness"), 1.0f);

			
		}
		else
			glUniform1f(glGetUniformLocation(shader, "dankness"), 1.0f);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//text
	}

	glUseProgram(0);
	glBindVertexArray(0);
}


UIElement& const Canvas::operator[](const char* _name)
{
	for (int i = uielements.size() - 1; i >= 0; i--)
	{
		if (strcmp(_name, uielements[i]->name) == 0)
		{
			return (*(uielements[i]));
		}
	}

	std::cout << "UIElement& const Canvas::operator[](const char* _name)::non-existent UIElement name" << std::endl;
	exit(-69);
}

*/