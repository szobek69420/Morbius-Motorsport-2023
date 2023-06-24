#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.h"

bool leaveGame = false;

int main(void)
{
	GLFWwindow* window = nullptr;

	window = InitWindow(1200, 600, "Morbius Motorsport 2023");
	if (window == nullptr) return -69;

menu:
	tempMenu2(window);
	if (leaveGame||glfwWindowShouldClose(window)) goto flee;
	realgame(window);
	if(!glfwWindowShouldClose(window)) goto menu;
flee:
	glfwTerminate();

	return 0;
}
