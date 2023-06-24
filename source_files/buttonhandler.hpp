#ifndef BUTTONHANDLER_HPP
#define BUTTONHANDLER_HPP

#include <GLFW/glfw3.h>
#include <cctype>

//egy namespace csak a blokkok neveinek elkulonitesehez (hogy ne szemeteljem tele a global namespace-t)
namespace Buttons {
	enum ButtonNames {
		BUTTON_A,
		BUTTON_B,
		BUTTON_C,
		BUTTON_D,
		BUTTON_E,
		BUTTON_F,
		BUTTON_G,
		BUTTON_H,
		BUTTON_I,
		BUTTON_J,
		BUTTON_K,
		BUTTON_L,
		BUTTON_M,
		BUTTON_N,
		BUTTON_O,
		BUTTON_P,
		BUTTON_Q,
		BUTTON_R,
		BUTTON_S,
		BUTTON_T,
		BUTTON_U,
		BUTTON_V,
		BUTTON_W,
		BUTTON_X,
		BUTTON_Y,
		BUTTON_Z,
		BUTTON_0,
		BUTTON_1,
		BUTTON_2,
		BUTTON_3,
		BUTTON_4,
		BUTTON_5,
		BUTTON_6,
		BUTTON_7,
		BUTTON_8,
		BUTTON_9,
		BUTTON_SPACE,
		BUTTON_ESCAPE,
		BUTTON_ENTER,
		BUTTON_LEFT_SHIFT,
		BUTTON_LEFT_CONTROL,
		BUTTON_MOUSE_LEFT,
		BUTTON_MOUSE_RIGHT,
		BUTTON_MOUSE_MIDDLE
	};
}

//handles english characters, numbers and mousebuttons
class ButtonHandler {
private:
	static enum class ButtonState {
		BUTTON_STATE_UNTOUCHED,//fel van engedve
		BUTTON_STATE_DOWN,//pont most nyomta le
		BUTTON_STATE_HELD,//le van tartva
		BUTTON_STATE_UP//pont most engedte fel
	};

	//a kulonfele kategoriaba tartozo gombok allapotat kulon tombokben mentem el, hogy ne zavarodjak bele olyan konnyen
	static ButtonState alphabet[26];
	static ButtonState numbers[10];
	static ButtonState specialButtons[5];//space, escape, leftshift, leftcontrol
	static ButtonState mouseButtons[3];
	//GLFW_MOUSE_BUTTON_1: bal egergomb
	//GLFW_MOUSE_BUTTON_2: jobb egergomb
	//GLFW_MOUSE_BUTTON_3: kozepso egergomb
	//ezert a mouseButtons[]-ban is igy lesz a sorrend

	//a specialis gombokhoz rendelt GLFW define-okat gyujti egy tombbe
	static int specialButtons_GLFW[5];
	static const unsigned int SPECIAL_BUTTON_COUNT = 5;

	//nullazza (BUTTON_STATE_UNTOUCHED) az osszes gomb allapotat
	static void Reset()
	{
		for (int i = 0; i < 26; i++)
			alphabet[i] = ButtonState::BUTTON_STATE_UNTOUCHED;

		for (int i = 0; i < 10; i++)
			numbers[i] = ButtonState::BUTTON_STATE_UNTOUCHED;

		for (int i = 0; i < 3; i++)
			mouseButtons[i] = ButtonState::BUTTON_STATE_UNTOUCHED;

		for (int i = 0; i < 4; i++)
			specialButtons[i] = ButtonState::BUTTON_STATE_UNTOUCHED;
	}

public:
	//csak azert kellett, hogy ne lehessen peldanyositani
	virtual ~ButtonHandler() = 0;

	//begyujti a gombok allapotat a GLFW keretrendszertol
	static void UpdateStates(GLFWwindow* window)
	{
		glfwPollEvents();

		//alphabet
		for (int i = 0; i <= (GLFW_KEY_Z - GLFW_KEY_A); i++)
		{
			if (glfwGetKey(window, i+GLFW_KEY_A) == GLFW_PRESS)
			{
				if (alphabet[i] == ButtonState::BUTTON_STATE_UNTOUCHED||alphabet[i]==ButtonState::BUTTON_STATE_UP)
					alphabet[i] = ButtonState::BUTTON_STATE_DOWN;
				else
					alphabet[i] = ButtonState::BUTTON_STATE_HELD;
			}
			else
			{
				if (alphabet[i] == ButtonState::BUTTON_STATE_HELD || alphabet[i]==ButtonState::BUTTON_STATE_DOWN)
					alphabet[i] = ButtonState::BUTTON_STATE_UP;
				else
					alphabet[i] = ButtonState::BUTTON_STATE_UNTOUCHED;
			}
		}

		//numbers
		for (int i = 0; i <= (GLFW_KEY_9 - GLFW_KEY_0); i++)
		{
			if (glfwGetKey(window, i+GLFW_KEY_0) == GLFW_PRESS)
			{
				if (numbers[i] == ButtonState::BUTTON_STATE_UNTOUCHED || numbers[i] == ButtonState::BUTTON_STATE_UP)
					numbers[i] = ButtonState::BUTTON_STATE_DOWN;
				else
					numbers[i] = ButtonState::BUTTON_STATE_HELD;
			}
			else
			{
				if (numbers[i] == ButtonState::BUTTON_STATE_HELD || numbers[i] == ButtonState::BUTTON_STATE_DOWN)
					numbers[i] = ButtonState::BUTTON_STATE_UP;
				else
					numbers[i] = ButtonState::BUTTON_STATE_UNTOUCHED;
			}
		}

		//mouse buttons
		for (int i = 0; i < 3; i++)
		{
			if (glfwGetMouseButton(window, i + GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				if (mouseButtons[i] == ButtonState::BUTTON_STATE_UNTOUCHED || mouseButtons[i] == ButtonState::BUTTON_STATE_UP)
					mouseButtons[i] = ButtonState::BUTTON_STATE_DOWN;
				else
					mouseButtons[i] = ButtonState::BUTTON_STATE_HELD;
			}
			else
			{
				if (mouseButtons[i] == ButtonState::BUTTON_STATE_HELD || mouseButtons[i] == ButtonState::BUTTON_STATE_DOWN)
					mouseButtons[i] = ButtonState::BUTTON_STATE_UP;
				else
					mouseButtons[i] = ButtonState::BUTTON_STATE_UNTOUCHED;
			}
		}

		//special buttons
		for (int i = 0; i < SPECIAL_BUTTON_COUNT; i++)
		{
			if (glfwGetKey(window, specialButtons_GLFW[i]) == GLFW_PRESS)
			{
				if (specialButtons[i] == ButtonState::BUTTON_STATE_UNTOUCHED || specialButtons[i] == ButtonState::BUTTON_STATE_UP)
					specialButtons[i] = ButtonState::BUTTON_STATE_DOWN;
				else
					specialButtons[i] = ButtonState::BUTTON_STATE_HELD;
			}
			else
			{
				if (specialButtons[i] == ButtonState::BUTTON_STATE_HELD || specialButtons[i] == ButtonState::BUTTON_STATE_DOWN)
					specialButtons[i] = ButtonState::BUTTON_STATE_UP;
				else
					specialButtons[i] = ButtonState::BUTTON_STATE_UNTOUCHED;
			}
		}
	}

	//eppen most lett lenyomva a gomb
	static bool GetKeyDown(Buttons::ButtonNames _button)
	{
		enum ButtonState _state = ButtonState::BUTTON_STATE_UNTOUCHED;

		if (_button >= Buttons::BUTTON_A && _button <= Buttons::BUTTON_Z)
			_state=alphabet[(unsigned)(_button - Buttons::BUTTON_A)];
		else if (_button >= Buttons::BUTTON_0 && _button <= Buttons::BUTTON_9)
			_state = numbers[(unsigned)(_button - Buttons::BUTTON_0)];
		else if (_button >= Buttons::BUTTON_MOUSE_LEFT && _button <= Buttons::BUTTON_MOUSE_MIDDLE)
			_state = mouseButtons[(unsigned)(_button - Buttons::BUTTON_MOUSE_LEFT)];
		else if (_button >= Buttons::BUTTON_SPACE && _button < Buttons::BUTTON_SPACE+SPECIAL_BUTTON_COUNT)
			_state = specialButtons[(unsigned)(_button - Buttons::BUTTON_SPACE)];

		if (_state == ButtonState::BUTTON_STATE_DOWN)
			return true;
		else
			return false;
	}

	//tartva van a gomb
	static bool GetKey(Buttons::ButtonNames _button)
	{
		enum ButtonState _state = ButtonState::BUTTON_STATE_UNTOUCHED;

		if (_button >= Buttons::BUTTON_A && _button <= Buttons::BUTTON_Z)
			_state = alphabet[(unsigned)(_button - Buttons::BUTTON_A)];
		else if (_button >= Buttons::BUTTON_0 && _button <= Buttons::BUTTON_9)
			_state = numbers[(unsigned)(_button - Buttons::BUTTON_0)];
		else if (_button >= Buttons::BUTTON_MOUSE_LEFT && _button <= Buttons::BUTTON_MOUSE_MIDDLE)
			_state = mouseButtons[(unsigned)(_button - Buttons::BUTTON_MOUSE_LEFT)];
		else if (_button >= Buttons::BUTTON_SPACE && _button < Buttons::BUTTON_SPACE + SPECIAL_BUTTON_COUNT)
			_state = specialButtons[(unsigned)(_button - Buttons::BUTTON_SPACE)];

		if (_state == ButtonState::BUTTON_STATE_HELD)
			return true;
		else
			return false;
	}

	//eppen most lett feleresztve a gomb
	static bool GetKeyUp(Buttons::ButtonNames _button)
	{
		enum ButtonState _state = ButtonState::BUTTON_STATE_UNTOUCHED;

		if (_button >= Buttons::BUTTON_A && _button <= Buttons::BUTTON_Z)
			_state = alphabet[(unsigned)(_button - Buttons::BUTTON_A)];
		else if (_button >= Buttons::BUTTON_0 && _button <= Buttons::BUTTON_9)
			_state = numbers[(unsigned)(_button - Buttons::BUTTON_0)];
		else if (_button >= Buttons::BUTTON_MOUSE_LEFT && _button <= Buttons::BUTTON_MOUSE_MIDDLE)
			_state = mouseButtons[(unsigned)(_button - Buttons::BUTTON_MOUSE_LEFT)];
		else if (_button >= Buttons::BUTTON_SPACE && _button < Buttons::BUTTON_SPACE + SPECIAL_BUTTON_COUNT)
			_state = specialButtons[(unsigned)(_button - Buttons::BUTTON_SPACE)];

		if (_state == ButtonState::BUTTON_STATE_UP)
			return true;
		else
			return false;
	}

};

ButtonHandler::~ButtonHandler(){}


enum ButtonHandler::ButtonState ButtonHandler::alphabet[26] = {
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED
};

enum ButtonHandler::ButtonState ButtonHandler::numbers[10] = {
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED
};

enum ButtonHandler::ButtonState ButtonHandler::mouseButtons[3] = {
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED
};

enum ButtonHandler::ButtonState ButtonHandler::specialButtons[5] = {
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED,
	ButtonHandler::ButtonState::BUTTON_STATE_UNTOUCHED
};

int ButtonHandler::specialButtons_GLFW[5] = {
	GLFW_KEY_SPACE,
	GLFW_KEY_ESCAPE,
	GLFW_KEY_ENTER,
	GLFW_KEY_LEFT_SHIFT,
	GLFW_KEY_LEFT_CONTROL
};

#endif