/*
#ifndef UI_H
#define UI_H

#include <vector>
#include <string.h>


enum uielement_type {
	PASSIVE,
	BUTTON
};

class Canvas;///canvas class prototype

class UIElement {

	friend Canvas;

private:
	enum uielement_type type;
	
	float size[2];
	float position[2];//bal also sarok helye


	float uvsize[2];
	float uvposition[2];//bal also sarok helye

	unsigned int background_texture;

	char text[100];
	unsigned int text_length;
	unsigned int text_texture;

	char name[100];

	void (*buttonCallback)(void);

public:
	UIElement(const char* _name="element");
	UIElement(void (*_buttonCallback)(void), unsigned int _background_texture, const char* _text = "", const char* _name = "element");
	UIElement(enum uielement_type _type, float _uvsizex, float _uvsizey, float _uvpositionx, float _uvpositiony, unsigned int _background_texture, const char* _text = "", const char* _name="element");
	~UIElement();

	void setPosition(float x, float y);
	void setSize(float x, float y);
	void setUVPosition(float x, float y);
	void setUVSize(float x, float y);

	void setBackground(unsigned int _background_texture);

	void setCallback(void(*_buttonCallback)(void));
};

class Canvas {
private:
	std::vector<UIElement*> uielements;
	bool visible;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	unsigned int shader;

public:
	Canvas(bool _visible=true);
	~Canvas();

	void AddElement(enum uielement_type _type, const char* _name, unsigned int _background_texture = 0, const char* _text = "");
	void AddElement(const char* _name, void(*_buttonCallback)(void), unsigned int _background_texture = 0, const char* _text = "");

	void Render(float cursorX, float cursorY, bool leftClick=false);

	UIElement& const operator[](const char* _name);
};


#endif
*/