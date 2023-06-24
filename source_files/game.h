#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* InitWindow(int width, int height, const char* windowname);

void tempMenu(GLFWwindow* window);
void tempMenu2(GLFWwindow* window);
void realgame(GLFWwindow* window);

#endif