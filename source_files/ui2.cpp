#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ui2.h"
#include "shader.h"

Character::~Character()
{
	//glDeleteTextures(1, &TextureID);
}

UIElement::UIElement(const char* _name)
{
	pos[0] = 0.0;
	pos[1] = 0.0;
	scale[0] = 10.0;
	scale[1] = 100.0;

	uvpos[0] = 0.0;
	uvpos[1] = 0.0;
	uvscale[0] = 1.0;
	uvscale[1] = 1.0;


	pos_current[0] = 0.0;
	pos_current[1] = 0.0;
	scale_current[0] = 10.0;
	scale_current[1] = 100.0;

	for (int i = 0; i < 4; i++)
	{
		baseColour[i] = 1.0f;
		hoverColour[i] = 0.5f;
	}
	hoverColour[3] = 1.0f;

	visible = true;


	interactable = false;

	callback_untouched = NULL;
	parameter_untouched = NULL;
	callback_down = NULL;
	parameter_down = NULL;
	callback_held = NULL;
	parameter_held = NULL;
	callback_up = NULL;
	parameter_up = NULL;


	strcpy_s(name, 100, _name);

	anchor = UI_BOTTOM_LEFT;
	stretch = UI_FIXED_ASPECT_Y;
}

void UIElement::Press(INTERACTION::InteractionType _interaction)
{
	if (!interactable)
		return;

	switch (_interaction)
	{
	case INTERACTION::INTERACTION_UNTOUCHED:
		if (callback_untouched != NULL) callback_untouched(parameter_untouched);
		break;

	case INTERACTION::INTERACTION_DOWN:
		if (callback_down != NULL) callback_down(parameter_down);
		break;

	case INTERACTION::INTERACTION_HELD:
		if (callback_held != NULL) callback_held(parameter_held);
		break;

	case INTERACTION::INTERACTION_UP:
		if (callback_up != NULL) callback_up(parameter_up);
		break;
	}
}

void UIElement::SetCallback(INTERACTION::InteractionType _interaction, void (*_callback)(void*), void* _parameter)
{
	switch (_interaction)
	{
	case INTERACTION::INTERACTION_UNTOUCHED:
		callback_untouched = _callback;
		parameter_untouched = _parameter;
		break;

	case INTERACTION::INTERACTION_DOWN:
		callback_down = _callback;
		parameter_down = _parameter;
		break;

	case INTERACTION::INTERACTION_HELD:
		callback_held = _callback;
		parameter_held = _parameter;
		break;

	case INTERACTION::INTERACTION_UP:
		callback_up = _callback;
		parameter_up = _parameter;
		break;

	default:
		std::cerr << "bruh" << std::endl;
		break;
	}
}

float UIElement::GetPosition(unsigned int index)
{
	if (index > 1)
	{
		std::cerr << "UIElement::GetPosition(unsigned int index): index out of range exception" << std::endl;
		exit(-69);
	}
	return pos[index];
}

float UIElement::GetScale(unsigned int index)
{
	if (index > 1)
	{
		std::cerr << "UIElement::GetScale(unsigned int index): index out of range exception" << std::endl;
		exit(-69);
	}
	return scale[index];
}


float UIElement::GetCurrentPosition(unsigned int index)
{
	if (index > 1)
	{
		std::cerr << "UIElement::GetPosition(unsigned int index): index out of range exception" << std::endl;
		exit(-69);
	}
	return pos_current[index];
}

float UIElement::GetCurrentScale(unsigned int index)
{
	if (index > 1)
	{
		std::cerr << "UIElement::GetScale(unsigned int index): index out of range exception" << std::endl;
		exit(-69);
	}
	return scale_current[index];
}


float UIElement::GetUVPosition(unsigned int index)
{
	if (index > 1)
	{
		std::cerr << "UIElement::GetPosition(unsigned int index): index out of range exception" << std::endl;
		exit(-69);
	}
	return uvpos[index];
}

float UIElement::GetUVScale(unsigned int index)
{
	if (index > 1)
	{
		std::cerr << "UIElement::GetScale(unsigned int index): index out of range exception" << std::endl;
		exit(-69);
	}
	return uvscale[index];
}


//image
void Image::Render()
{
	if (visible == false) return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

//text
std::map<GLchar, Character> Text::Characters = {};
GLuint Text::text_model_matrix_location = 0;

void Text::CalculateFontScale(float _scaleX, float _scaleY)
{
	int length = strlen(text);

	//rescaling font
	/*float textWidth = 0.0;
	for (unsigned int i = 0; i < length; i++)
		textWidth += (Characters[text[i]].Advance >> 6);*/

	//fontScale[0] = _scaleX / textWidth;
	
	fontScale[1] = (_scaleY / FONT_HEIGHT)*0.75f;
	if (stretchToScale) fontScale[0] = _scaleX / (length * (float)FONT_WIDTH);
	else fontScale[0] = fontScale[1];
}

void Text::Render()
{
	if (visible == false) return;
	if (strcmp(text, "") == 0) return;

	//std::cout << this->text << std::endl;

	//getting data
	float _pos[2];
	float _scale[2];

	_pos[0] = this->GetCurrentPosition(0);
	_pos[1] = this->GetCurrentPosition(1);
	_scale[0] = this->GetCurrentScale(0);
	_scale[1] = this->GetCurrentScale(1);
	

	glActiveTexture(GL_TEXTURE0);

	// iterate through all characters
	int length = strlen(text);
	float textWidth = length * FONT_WIDTH * fontScale[0];;
	
	switch (horizontalAlignment)
	{
	case TEXT_LEFT:
		_pos[0] = _pos[0];//balra igazitasnal valtozatlan marad a szoveg kezdete
		break;

	case TEXT_RIGHT:
		_pos[0] = _pos[0] + _scale[0] - textWidth;
		break;

	case TEXT_CENTERED:
		_pos[0] = _pos[0] + _scale[0] / 2.0f - textWidth / 2.0f;
		break;
	}

	switch (verticalAlignment)
	{
	case TEXT_TOP:
		_pos[1] = _pos[1] + _scale[1] - fontScale[1] * FONT_HEIGHT;
		break;

	case TEXT_BOTTOM:
		_pos[1] = _pos[1];
		break;

	case TEXT_MIDDLE:
		_pos[1] = (_pos[1] + _scale[1] / 2.0f) - FONT_HEIGHT *fontScale[1] / 2.0f;
		break;
	}

	for (GLchar i = 0; i < length; i++)
	{
		Character ch = Characters[text[i]];

		float xpos = _pos[0] + ch.Bearing.x * fontScale[0];
		float ypos = _pos[1] - (ch.Size.y - ch.Bearing.y) * fontScale[1];


		float w = ch.Size.x * fontScale[0];
		float h = ch.Size.y * fontScale[1];
		// send model matrix to shader
		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(xpos, ypos, 0.0));
		model = glm::scale(model, glm::vec3(w, h, 1.0));
		glUniformMatrix4fv(Text::text_model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));


		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		// render quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		_pos[0] += (ch.Advance >> 6)* fontScale[0]; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::ImportTTF(const char* name)
{
	//text import (pure stolen code)

	// FreeType
	// --------
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		exit(-1);
	}
	// find path to font
	char font_name[100];
	strcpy_s(font_name, 99, name);

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font_name, 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		exit(-1);
	}
	else {
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, FONT_WIDTH, FONT_HEIGHT);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Text::DestroyTTF()
{
	for (unsigned char c = 0; c < 128; c++)//deleting fonts
	{
		glDeleteTextures(1, &Characters[c].TextureID);
		Characters[c].TextureID = 0;
	}

	Characters.clear();
}

//canvas

Canvas::Canvas()
{
	//data
	screen[0] = 1920;
	screen[1] = 1080;

	defaultScreen[0] = 1920;
	defaultScreen[1] = 1080;

	visible = true;

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

	shader_image = createShader(SHADER_UI_IMAGE);
	shader_text = createShader(SHADER_UI_TEXT);

	glUseProgram(shader_image);
	glUniform1i(glGetUniformLocation(shader_image, "Texture"), 0);
	glUniform1f(glGetUniformLocation(shader_image, "dankness"), 1.0f);
	glUseProgram(0);

	glUseProgram(shader_text);
	glUniform1i(glGetUniformLocation(shader_text, "Texture"), 0);
	glUniform1f(glGetUniformLocation(shader_text, "dankness"), 1.0f);
	glUseProgram(0);
}

Canvas::~Canvas()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	destroyShader(shader_image);
	destroyShader(shader_text);

	vao = 0;
	vbo = 0;
	ebo = 0;
	shader_image = 0;
	shader_text = 0;

	for (int i = 0; i < uielements.size(); i++)
		delete uielements[i];
	uielements.clear();
}


void Canvas::Render(float cursorX, float cursorY, bool leftClick_down, bool leftClick_held, bool leftClick_up)
{
	if (visible == false)
		return;

	unsigned int length = uielements.size();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader_image);//ez jo a text-nek is elvileg
	glBindVertexArray(vao);

	GLuint model_location = glGetUniformLocation(shader_image, "model");
	Text::SetModelMatrixLocation(model_location);
	GLuint uvmodel_location = glGetUniformLocation(shader_image, "uvmodel");
	GLuint texture_location= glGetUniformLocation(shader_image, "Texture");
	GLuint colour_location = glGetUniformLocation(shader_image, "colour");
	GLuint isText_location = glGetUniformLocation(shader_image, "isText");
	GLuint isText2_location = glGetUniformLocation(shader_image, "isText2");

	//making projection matrix
	glm::mat4 projection = glm::ortho(0.0f, screen[0], 0.0f, screen[1]);
	glUniformMatrix4fv(glGetUniformLocation(shader_image, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform1i(texture_location, 0);

	float _pos[2];
	float _scale[2];
	float _uvpos[2];
	float _uvscale[2];

	float r, g, b, a;

	for (int i = 0; i < length; i++)
	{
		_pos[0] = uielements[i]->GetCurrentPosition(0);
		_pos[1] = uielements[i]->GetCurrentPosition(1);
		_scale[0] = uielements[i]->GetCurrentScale(0);
		_scale[1] = uielements[i]->GetCurrentScale(1);

		_uvpos[0] = uielements[i]->GetUVPosition(0);
		_uvpos[1] = uielements[i]->GetUVPosition(1);
		_uvscale[0] = uielements[i]->GetUVScale(0);
		_uvscale[1] = uielements[i]->GetUVScale(1);

		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(_pos[0], _pos[1], 0.0));
		model = glm::scale(model, glm::vec3(_scale[0], _scale[1], 1.0));

		glm::mat4 uvmodel = glm::translate(glm::mat4(1.0), glm::vec3(_uvpos[0], _uvpos[1], 0.0));
		uvmodel = glm::scale(uvmodel, glm::vec3(_uvscale[0], _uvscale[1], 1.0));

		switch (uielements[i]->GetType())
		{
		case UI_IMAGE:
			glUniform1i(isText_location, 0);
			glUniform1i(isText2_location, 0);
			break;

		case UI_TEXT:
			glUniform1i(isText_location, 1);
			glUniform1i(isText2_location, 1);
			break;
		}

		if (cursorX > _pos[0] && cursorX<_pos[0] + _scale[0] && cursorY>_pos[1] && cursorY < _pos[1] + _scale[1])
		{
			if (uielements[i]->isInteractable())
			{
				uielements[i]->GetHoverColour(r, g, b, a);
				
				if(leftClick_down) 
					uielements[i]->Press(INTERACTION::INTERACTION_DOWN);
				if (leftClick_held)
					uielements[i]->Press(INTERACTION::INTERACTION_HELD);
				if (leftClick_up)
					uielements[i]->Press(INTERACTION::INTERACTION_UP);
				if(!(leftClick_down||leftClick_held||leftClick_up))
					uielements[i]->Press(INTERACTION::INTERACTION_UNTOUCHED);
			}
			else
				uielements[i]->GetBaseColour(r, g, b, a);
		}
		else
			uielements[i]->GetBaseColour(r, g, b, a);

		glUniform4f(colour_location, r, g, b, a);

		glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uvmodel_location, 1, GL_FALSE, glm::value_ptr(uvmodel));


		uielements[i]->Render();
	}

	glDisable(GL_BLEND);

	glUseProgram(0);
	glBindVertexArray(0);
}


void Canvas::AddElement(enum uielement_type_real _type, const char* _name)
{
	UIElement* temp;
	switch (_type)
	{
	case UI_IMAGE:
		temp = new Image(_name);
		uielements.push_back(temp);
		break;

	case UI_TEXT:
		temp = new Text(_name);
		uielements.push_back(temp);
		break;
	}
}

void Canvas::SetScreenSize(float x, float y)
{ 
	screen[0] = x; 
	screen[1] = y; 


	float aspect_x = screen[0] / defaultScreen[0];
	float aspect_y = screen[1] / defaultScreen[1];
	float defaultAspectRatio = defaultScreen[0] / defaultScreen[1];
	float newAspectRatio = screen[0] / screen[1];

	int length = uielements.size();

	float posX, posY, scaleX, scaleY, orgScaleX, orgScaleY;

	for (int i = 0; i < length; i++)
	{
		posX = uielements[i]->GetPosition(0);
		posY = uielements[i]->GetPosition(1);
		scaleX = uielements[i]->GetScale(0);
		scaleY = uielements[i]->GetScale(1);

		orgScaleX = scaleX;
		orgScaleY = scaleY;

		//stretch
		switch (uielements[i]->GetStretch())
		{
		case UI_STRETCH:
			scaleX *= aspect_x;
			scaleY *= aspect_y;
			break;

		case UI_FIXED_ASPECT_X:
			scaleX *= aspect_x;
			scaleY *= aspect_y * (newAspectRatio / defaultAspectRatio);
			break;

		case UI_FIXED_ASPECT_Y:
			scaleY *= aspect_y;
			scaleX *= aspect_x * (defaultAspectRatio / newAspectRatio);
			break;
		}

		//alignment
		switch (uielements[i]->GetAnchor())
		{
		case UI_BOTTOM_CENTER:
			posX = (posX + (orgScaleX / 2.0f)) * aspect_x - (scaleX / 2.0f);
			posY = (posY + (orgScaleY / 2.0f)) * aspect_y - (scaleY / 2.0f);
			break;

		case UI_BOTTOM_LEFT:
			posX = aspect_x * posX;
			posY = aspect_y * posY;
			break;

		case UI_BOTTOM_RIGHT:
			posX = (posX + orgScaleX) * aspect_x - scaleX;
			posY = (posY + orgScaleY) * aspect_y - scaleY;
			break;
		}

		uielements[i]->SetCurrent(posX, posY, scaleX, scaleY);
	
		if (uielements[i]->GetType() == UI_TEXT)
		{
			((Text*)uielements[i])->CalculateFontScale(scaleX, scaleY);
		}
	}
}


UIElement* const Canvas::operator[](const char* _name)
{
	for (int i = uielements.size() - 1; i >= 0; i--)
	{
		if (strcmp(_name, uielements[i]->name) == 0)
		{
			return (uielements[i]);
		}
	}

	std::cout << "UIElement& const Canvas::operator[](const char* _name)::non-existent UIElement name" << std::endl;
	exit(-69);
}