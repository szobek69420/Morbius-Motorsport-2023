#ifndef UI2_H//NEM SZABAD "ui.h"-VAL EGYUTT INCLUDEOLNI, KULONBEN DURVA UTKOZESEK LESZNEK
#define UI2_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <map>
#include "my_vector.hpp"

/*enum uielement_type_2 {
	PASSIVE,
	INTERACTABLE
};*/

//az adott ui elem tipusa
enum uielement_type_real {
	UI_IMAGE, //kulso texturat var
	UI_TEXT //a betoltott font alapjan general texturat
};

//az adott ui elem hogyan viselkedik, ha valtozik a kepernyo merete (es aranya)
enum uielement_stretch_type {
	UI_STRETCH, //a kepernyo aranytol fuggetlenul valtoztatja a meretet
	UI_FIXED_ASPECT_Y, //az elem oldalaranya es a kepernyomagassag/elemmagassag konstans
	UI_FIXED_ASPECT_X //az elem oldalaranya es a kepernyoszelesseg/elemszelesseg konstans
};

//which part of the uielement has a fixed position
enum uielement_alignment_type {
	UI_BOTTOM_CENTER,//az elem also kozepso pontjanak van fix helye
	UI_BOTTOM_LEFT,//az elem bal also sarkanak van fix helye
	UI_BOTTOM_RIGHT//az elem jobb also sarkanak van fix helye
};

//a szoveg vizszintes igazitasa az elemen belul
enum text_alignment_horizontal {
	TEXT_LEFT,
	TEXT_RIGHT,
	TEXT_CENTERED
};

//a szoveg fuggoleges igazitasa az elemen belul
enum text_alignment_vertical {
	TEXT_TOP,
	TEXT_BOTTOM,
	TEXT_MIDDLE
};

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph

	~Character();
};

namespace INTERACTION {
	enum InteractionType {
		INTERACTION_UNTOUCHED,
		INTERACTION_DOWN,
		INTERACTION_HELD,
		INTERACTION_UP
	};
}


class Canvas;///canvas class prototype

class UIElement {
	friend Canvas;
protected:
	//identifier amongst all canvas elements
	char name[100];

	//both in pixels, (0.0,0.0) is the bottom left corner
	float pos[2];
	float scale[2];

	float uvpos[2];
	float uvscale[2];

	//current state
	float pos_current[2];
	float scale_current[2];

	//kis leiras az elem helyenek kiszamitasahoz:
	//	a canvas tartalmaz egy defaulScreen[] tagot, mely azt tarolja, hogy milyen kepernyomerethez lettek beallitva az elemek
	//	a pos[] es a scale[] a defaultScreen[]-hez viszonyitott helyzetet tarolja
	//	a canvas screen[] tagja tarolja az aktualis kepernyomereteket
	//	a pos_current[] es a scale_current[] a screen[]-hez van atszamolva ugy, hogy az aranyok a defaultScreen[]-nel beallitottakhoz hasonuljanak
	//	az atszamolasnal az anchor es a stretch is figyelembe van veve

	//colour (lathatosag es szinek)
	float baseColour[4];
	float hoverColour[4];
	bool visible;

	//alignment
	enum uielement_alignment_type anchor;//fixalt resze az elemnek
	enum uielement_stretch_type stretch;//elem oldalaranyait kezeli

	//if it's a button
	bool interactable;//if mouse hovers over it, it gets darker

	void (*callback_untouched)(void*);//callback function, if the uielement is interactable and the left click is pressed
	void* parameter_untouched;//egy tetszoleges parameter, mely kell a fuggvenyhez (callback-tol fuggoen ez lehet NULL)
	void (*callback_down)(void*);//callback function, if the uielement is interactable and the left click is pressed
	void* parameter_down;//egy tetszoleges parameter, mely kell a fuggvenyhez (callback-tol fuggoen ez lehet NULL)
	void (*callback_held)(void*);//callback function, if the uielement is interactable and the left click is pressed
	void* parameter_held;//egy tetszoleges parameter, mely kell a fuggvenyhez (callback-tol fuggoen ez lehet NULL)
	void (*callback_up)(void*);//callback function, if the uielement is interactable and the left click is pressed
	void* parameter_up;//egy tetszoleges parameter, mely kell a fuggvenyhez (callback-tol fuggoen ez lehet NULL)

public:
	//egy direkt teljesen egyszeru konstruktor
	//alaphelyzetbe allitja az osszes tagvaltozot
	UIElement(const char* _name="");
	virtual ~UIElement() {}

	//kell, hogy ne legyen peldanyosithato
	virtual void Render() = 0;

	//gombnyomasnal a callback fuggveny meghivasa
	void Press(INTERACTION::InteractionType _interaction);

	//getter/setters
	void SetPosition(float x, float y) { pos[0] = x; pos[1] = y; }
	float GetPosition(unsigned int index);
	void GetPosition(float& x, float& y) { x = pos[0]; y = pos[1]; }

	void SetScale(float x, float y) { scale[0] = x; scale[1] = y; }
	float GetScale(unsigned int index);



	//void SetCurrentPosition(float x, float y) { pos_current[0] = x; pos_current[1] = y; }
	float GetCurrentPosition(unsigned int index);

	//void SetCurrentScale(float x, float y) { scale_current[0] = x; scale_current[1] = y; }
	float GetCurrentScale(unsigned int index);


	bool isVisible() { return visible; }
	void SetVisibility(bool _visible) { visible = _visible; }


	void SetUVPosition(float x, float y) { uvpos[0] = x; uvpos[1] = y; }
	float GetUVPosition(unsigned int index);

	void SetUVScale(float x, float y) { uvscale[0] = x; uvscale[1] = y; }
	float GetUVScale(unsigned int index);

	void SetCurrent(float posX, float posY, float scaleX, float scaleY)
	{
		pos_current[0] = posX;
		pos_current[1] = posY;
		scale_current[0] = scaleX;
		scale_current[1] = scaleY;
	}

	bool isInteractable() { return interactable; }
	void SetInteractable(bool _value) { interactable = _value; }
	
	void SetCallback(INTERACTION::InteractionType _interaction, void (*_callback)(void*), void* _parameter);
	//void SetCallbackParameter(void* _parameter) { parameter = _parameter; }


	void SetBaseColour(float r, float g, float b, float a = 1.0) { baseColour[0] = r; baseColour[1] = g; baseColour[2] = b; baseColour[3] = a; }
	void GetBaseColour(float& r, float& g, float& b, float& a) { r = baseColour[0]; g = baseColour[1]; b = baseColour[2]; a = baseColour[3]; }

	void SetHoverColour(float r, float g, float b, float a = 1.0) { hoverColour[0] = r; hoverColour[1] = g; hoverColour[2] = b; hoverColour[3] = a; }
	void GetHoverColour(float& r, float& g, float& b, float& a) { r = hoverColour[0]; g = hoverColour[1]; b = hoverColour[2]; a = hoverColour[3]; }




	void SetAnchor(enum uielement_alignment_type _anchor) { anchor = _anchor; }
	enum uielement_alignment_type GetAnchor() { return anchor; }

	void SetStretch(enum uielement_stretch_type _stretch) { stretch = _stretch; }
	enum uielement_stretch_type GetStretch() { return stretch; }

	virtual enum uielement_type_real GetType() = 0;
};


//egy ui elembol szarmaztatott osztaly, mely texturak megjelenitesere alkalmas
class Image :public UIElement {

	friend Canvas;

private:
	//egy kulso texture, nem itt lesz generalva
	GLuint texture;

public:
	Image(const char* _name, unsigned int _texture = 0) :UIElement(_name), texture(_texture) { }
	virtual ~Image() {}

	virtual void Render();

	//getter-setters
	GLuint GetTexture() { return texture; }
	void SetTexture(GLuint _texture) { texture = _texture; }

	virtual enum uielement_type_real GetType() { return UI_IMAGE; }
};

//egy ui elembol szarmaztatott osztaly, mely szovegbol texturat general
class Text : public UIElement {
	
	friend Canvas;

private:
	char text[120];//azert csak 120, mert a kiiratasnal tulcsordulna

	float fontScale[2];
	void SetFontScale(float x, float y) { fontScale[0] = x; fontScale[1] = y; }

	//szovegigazitas
	bool stretchToScale;//a uielem meretevel aranyosan nyulik a szoveg
	enum text_alignment_horizontal horizontalAlignment;
	enum text_alignment_vertical verticalAlignment;

	static GLuint text_model_matrix_location;//kell a kirajzolashoz
	static std::map<GLchar, Character> Characters;//importalt karaketereket tarolo hashmap

	//a szoveg importalasanal egy karakter felbontasa (fixaltam, mert mashogy nem mukodott)
	static const int FONT_HEIGHT = 32;
	static const int FONT_WIDTH = 16;

public:
	Text(const char* _name) :UIElement(_name), stretchToScale(false), horizontalAlignment(TEXT_LEFT), verticalAlignment(TEXT_BOTTOM) { strcpy_s(text, 120, ""); fontScale[0] = 1.0f; fontScale[1] = 1.0f; }
	virtual ~Text() {}

	virtual void Render();

	virtual enum uielement_type_real GetType() { return UI_TEXT; }

	void SetStretchToScale(bool _stretchToScale) { stretchToScale = _stretchToScale; }

	void SetText(const char* _text) { strcpy_s(text, 120, _text); }
	void SetTextAlignment(enum text_alignment_horizontal _horizontal, enum text_alignment_vertical _vertical) { horizontalAlignment = _horizontal; verticalAlignment = _vertical; }

	//megnezi, hogy milyen szeles a szoveget tartalmazo ui elem es az alapjan kiszamolja, hogy milyen szelesek az egyes karakterek
	void CalculateFontScale(float _scaleX, float _scaleY);

	//peldanyositastol fuggetlenul importal egy TrueTypeFonts betukeszletet (pure stolen code)
	static void ImportTTF(const char* name);
	static void DestroyTTF();

	static void SetModelMatrixLocation(GLuint _location) { text_model_matrix_location = _location; }
};

class Canvas {
private:
	//megjeleniteshez szukseges valtozok
	unsigned int shader_image;
	unsigned int shader_text;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	//heterogenic collection
	gigachad::vector<UIElement*> uielements;

	//screen size
	float screen[2];//x, y, in pixels
	float defaultScreen[2];//the default screen size, to which the pixel coordinates were set

	bool visible;

public:
	Canvas();
	~Canvas();

	void Render(float cursorX, float cursorY, bool leftClick_down = false, bool leftClick_held = false, bool leftClick_up = false);

	void AddElement(enum uielement_type_real _type, const char* _name="");

	//getter-setters

	//EZT MINDEN KEPERNYOELEM POZICIOJANAK/MERETENEK MODOSITASA UTAN MEG KELL HIVNI!!!!
	void SetScreenSize(float x, float y);//nem csak atallitja a kepernyomeretet, hanem a canvas ui elemeinek is frissiti a mereteit es pozicioja

	void SetVisibility(bool _visible) { visible = _visible; }
	bool IsVisible() { return visible; }

	float GetDefaultScreenSize(unsigned int _index)
	{
		if (_index < 0 || _index>1)
			return 0.0f;
		return defaultScreen[_index];
	}

	//operatoruberladungen

	//egy elegansabb modja a uielem lekerdezesenek
	UIElement* const operator[](const char* _name);
};

#endif