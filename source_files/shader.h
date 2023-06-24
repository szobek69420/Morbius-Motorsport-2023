#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

//a shadertipusokat tartalmazo enumerator
enum shadertype {
	SHADER_CUBE,
	SHADER_WATER,
	SHADER_SELECTION,
	SHADER_UI_OLD,
	SHADER_UI,
	SHADER_UI_IMAGE,
	SHADER_UI_TEXT,
	SHADER_SKY,
	SHADER_HELD_ITEM,
	SHADER_MODEL,
	SHADER_SPRITE
};

//egy absztrakcios reteget biztosit az idegesitoen hosszu shader forditasi folyamathoz
GLuint createShader(enum shadertype shader);

//kitorli minden maradvanyat egy shadernek
void destroyShader(unsigned int shader);

class TextureImporter
{
public:
	TextureImporter() {}
	~TextureImporter() {}

	//eleresi ut alapjan importalhatunk texturakat
	static GLuint CreateTexture(const char* textureName, bool RGBA);
	
	//torli az importalt texturat a memoriabol
	static void DestroyTexture(unsigned int texture);
};

#endif