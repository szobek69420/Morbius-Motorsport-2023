#define _GLIBCXX_USE_NANOSLEEP
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <future>
#include <cmath>
#include <ft2build.h>
#include <thread>
#include "game.h"
#include "shader.h"
#include "camera.h"
#include "chunk.h"
#include "chunkmanager.h"
#include "physics.h"
#include "blockdatabase.h"
#include "audiomanager.h"
#include "sky.h"
#include "ui2.h"
#include "hand.h"
#include "model.h"
#include "buttonhandler.hpp"
#include "sprite.hpp"
#include "bullet.hpp"



static const int RENDER_DISTANCE = 4;
static const float CLIP_DISTANCE_FAR = 200.0F;
static const float CLIP_DISTANCE_NEAR = 0.05F;

unsigned int loadedBlockTextures[4];
unsigned int blockTexture;

//time measurement
double currentTime;

double lastFrame = 0.0f;
double deltaTime = 0.0f;
double lastSecond = 0.0f;
int lastFrameCount = 0;
int frameCount = 0;
int randomFrameCount = 0;//az fps szamlalo utso 3 szamjegyeert felel

double lastFixedUpdate = 0.0f;
double fixedDeltaTime = 0.0f;

double lastChunkLoad = 0.0f;

//input
bool viewportChanged = false;
int viewportWidth = 1200;
int viewportHeight = 600;
float aspectRatio = 2.0f;
float lastX = 600.0f;
float lastY = 300.0f;

bool firstMouse = true;

void* cam_voidp = nullptr;//kamerara mutato pointer csak a mousecallbacknek (void* mert jo)

int polygonMode = GL_FILL;


const float BLOCK_PLACEMENT_COOLDOWN = 0.2f;
bool leftPressed = false; bool hasBroken = false; float lastBlockBreak = 0.0f;
bool rightPressed = false; bool hasPlaced = false; float lastBlockPlacement = 0.0f;
int CPS = 0, lastCPS = 0;//left clicks per second


bool jumpStarted = false;

bool inSprint = false;

bool inGodMode = false;
bool godModeInitiated = false;
bool godModeAborted = false;

bool isCameraInWater = true;
bool isPlayerInWater[2] = { false,false };//a jatekos egy alsobb(0) es felsobb (1) pontjat ellenorzi, hogy benne van-e a vizben
Physics::CollisionType collisionType = Physics::COLLISION_NONE;//egy adott physics update-ben hogyan utkozott

int currentBlock = 0;
int hotbar[] = { AIR,AIR,AIR,AIR,AIR,AIR,AIR,AIR,AIR };
bool slotChanged = false;
bool hotbarContentChanged = false;

int inventory[27]={
	STONE, GRASS_BLOCK, SNOW_BLOCK, SAND, GLASS_BLOCK, BEDROCK, COBBLESTONE, OAK_LOG,OAK_LEAVES,
	BIRCH_LOG, BIRCH_LEAVES, RED_FLOWER, YELLOW_FLOWER, BLUE_FLOWER, AIR, AIR, AIR, AIR,
	DIAMOND_BLOCK, GOLD_BLOCK, IRON_BLOCK,COAL_BLOCK,EMERALD_BLOCK,NETHER_REACTOR_CORE, AMETHYST_BLOCK, AIR ,BORSOD
};

//menu
enum menu { MENU_INGAME, MENU_PAUSE, MENU_INVENTORY };
enum menu currentmenu = MENU_INGAME;


//texts
const float TEXT_REFRESH_INTERVAL = 0.1f;
float lastTextRefresh = 0.0;

//menu variables (callbacks after the prototypes)
double cursorPos[2] = { 0,0 };
bool hasStarted = false;
bool hasQuit = false;

bool gameQuit = false;//quit button pressed in pause menus


//gun

const float GEPFEGYVER_COOLDOWN = 0.1f;
float lastGepfegyver = 0.0f;
//az inGepfegyver azt jelenti, hogy aktiv-e a gepfegyver
bool inGepfegyver = false;//a slot change-nel (canvas render elott) van beallitva
bool inGepfegyverChanged = false;

enum activate_gepfegyver{
	ACTIVATE_,
	ACTIVATE_M,
	ACTIVATE_MO,
	ACTIVATE_MOR,
	ACTIVATE_MORB,
	ACTIVATE_MORBI,
	ACTIVATE_MORBIU,
	ACTIVATE_MORBIUS
};
enum activate_gepfegyver gepfegyverActivationState = ACTIVATE_;
const char* gepfegyverActivationText = "MORBIUS";

//animation
const unsigned int SPECULAR_ANIMATION_LENGTH = 6;
const int SPECULAR_ANIMATION_SPEED = 50;//hany fixed update lefutas kell ahhoz, hogy valtozzon a specular map
int updatesSinceSpecularChange = 0;//fixed update-hez kotom az animaciot
int currentSpecularMap = 0;


//camera constants
const float MOVEMENT_SPEED = 3.0f;
const float MOVEMENT_SPEED_WATER = 1.0f;
const float SPRINT_MOVEMENT_SPEED_MULTIPLIER = 2.0f;
const float JUMP_SPEED = 7.0f;
const float MOUSE_SENSITIVITY = 0.08f;

float currentFOV = NORMAL_FOV;
bool inZoom = false;

//render variables
glm::vec4 clearColor = glm::vec4(0.216f, 0.659f, 0.894f, 1.0f);
glm::vec4 shallowWater = glm::vec4(0.0f, 0.66f, 0.953f, 1.0f);
glm::vec4 deepWater = glm::vec4(0.0f, 0.04f, 0.337f, 1.0f);

//prototypes
void framebufferResizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseCallback_notFixed(GLFWwindow* window, double xpos, double ypos);
void SetMouseCallback(GLFWwindow* window, bool fixed, bool hide)
{
	if(fixed)
		glfwSetCursorPosCallback(window, mouseCallback);
	else
		glfwSetCursorPosCallback(window, mouseCallback_notFixed);

	if(hide)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
float SmoothLerp(float a, float b, float speed);

void processInput_NonPhysics_ingame();
void processInput_MenuChange(GLFWwindow* window);
glm::vec3 processMovementInput(GLFWwindow* window, Camera& cam);
bool processGepfegyverActivationInput(enum activate_gepfegyver& _activationstage, bool& _inGepfegyver);

void RefreshHotbar(Canvas& canvas);//only works if the "slot1"-"slot9" images already exist

void defaultGlobalVariables_Menu();
void defaultGlobalVariables_Game();

void realgame(GLFWwindow* window);
void tempMenu(GLFWwindow* window);


//callbacks
void startButton_menu(void* _p) { hasStarted = true; printf("started\n"); }
void startButton_game(void* _p) { SetMouseCallback((GLFWwindow*)_p, true, true); currentmenu = MENU_INGAME; }
extern bool leaveGame;
void quitButton_menu(void* _p) { hasQuit = true; leaveGame = true; }
void quitButton_game(void* _pGLFWwindow) { gameQuit = true; /*eredetileg a glfwWindowShouldClose-t hasznalta*/ }

int draggedItem = 0;
bool dragStarted = false;  bool inDrag = false;//ez a harom valtozo azert van itt, mert nagyon szorosan kapcsolodnak az alabbi ketto callback-hez
void inventorySlot(void* _p) { draggedItem = (int)_p; inDrag = true; dragStarted = true; }
void inventoryHotbarSlot(void* _p) { inDrag = false; if (draggedItem != 0) { hotbar[(int)_p] = draggedItem; hotbarContentChanged = true; } }
//program

GLFWwindow* InitWindow(int width, int height, const char* windowname)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, windowname, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	//glViewport(0, 0, width, height);
	framebufferResizeCallback(window, width, height);

	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	//glfwSetCursorPosCallback(window, mouseCallback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	//GPU
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	std::cout << vendor << " | " << renderer << "\n";

	return window;
}

//unused
void tempMenu(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, nullptr);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	unsigned int uishader = createShader(SHADER_UI_OLD);

	unsigned int morbius = TextureImporter::CreateTexture("morbius_poster_2.png", true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, morbius);

	glUseProgram(uishader);
	glUniform1i(glGetUniformLocation(uishader, "ui_texture"), 0);

	float vertices[]={
		-1.0,-1.0,0.0,0.0,
		-1.0,1.0,0.0,1.0,
		1.0,1.0,1.0,1.0,
		1.0,-1.0,1.0,0.0
	};
	unsigned int indices[] = { 0,2,1,0,3,2 };

	unsigned int vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));//texture
	glEnableVertexAttribArray(1);

	/*GLint size;

	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	std::cout << "vertex buffer: " << size << " "<<sizeof(vertices)<<" ";
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	std::cout << "element array buffer: " << size <<" "<<sizeof(indices)<< "\n";*/

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 1.0, 0.0, 1.0);
	while (glfwGetKey(window,GLFW_KEY_SPACE) != GLFW_PRESS)
	{
		//printf("macska\n");
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(uishader);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	destroyShader(uishader);
	TextureImporter::DestroyTexture(morbius);
}

void tempMenu2(GLFWwindow* window)
{
	SetMouseCallback(window, false, false);
	//glfwSetCursorPosCallback(window, mouseCallback_notFixed);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	void defaultGlobalVariables_Menu();
	currentmenu = MENU_INGAME;

	//audio

	AudioManager audio;
	audio.PlaySFX(AM_MUSIC_1_QUIET, true);


	//uisetup
#pragma region UI_SETUP_MENU
	unsigned int morbius = TextureImporter::CreateTexture("_sprites/morbius_poster_4.png", true);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int uiTexture = TextureImporter::CreateTexture("_sprites/ui_textures_8.png", true);

	Text::ImportTTF("_fonts/Adore64.ttf");

	Canvas canvas;
	canvas.AddElement(UI_IMAGE, "morbius_bg");
	canvas["morbius_bg"]->SetPosition(0.0, 0.0);
	canvas["morbius_bg"]->SetScale(1920.0,1080.0);
	canvas["morbius_bg"]->SetStretch(UI_STRETCH);
	canvas["morbius_bg"]->SetBaseColour(0.7f, 0.7f, 0.7f, 1.0f);
	((Image*)canvas["morbius_bg"])->SetTexture(morbius);
	
	canvas.AddElement(UI_IMAGE,"Start");
	canvas["Start"]->SetPosition(50.0, 400.0);
	canvas["Start"]->SetScale(480.0, 120.0);
	canvas["Start"]->SetUVPosition(0.4, 0.8);
	canvas["Start"]->SetUVScale(0.4, 0.1);
	((Image*)canvas["Start"])->SetTexture(uiTexture);
	canvas["Start"]->SetInteractable(true);
	canvas["Start"]->SetHoverColour(0.0f, 1.0f, 0.0f);
	canvas["Start"]->SetCallback(INTERACTION::INTERACTION_UP,startButton_menu,NULL);

	canvas.AddElement(UI_IMAGE, "Quit");
	canvas["Quit"]->SetPosition(50.0, 200.0);
	canvas["Quit"]->SetScale(480.0, 120.0);
	canvas["Quit"]->SetUVPosition(0.4, 0.8);
	canvas["Quit"]->SetUVScale(0.4, 0.1);
	((Image*)canvas["Quit"])->SetTexture(uiTexture);
	canvas["Quit"]->SetInteractable(true);
	canvas["Quit"]->SetHoverColour(1.0f, 0.0f, 0.0f);
	canvas["Quit"]->SetCallback(INTERACTION::INTERACTION_UP, quitButton_menu,NULL);

	canvas.AddElement(UI_TEXT, "MorbiusText");
	canvas["MorbiusText"]->SetPosition(480.0, 850.0);
	canvas["MorbiusText"]->SetScale(1400.0, 200.0);
	canvas["MorbiusText"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas["MorbiusText"]->SetBaseColour(0.0f, 1.0f, 1.0f, 1.0f);
	((Text*)canvas["MorbiusText"])->SetText("MORBIUS MOTORSPORT");
	((Text*)canvas["MorbiusText"])->SetTextAlignment(TEXT_RIGHT, TEXT_MIDDLE);

	canvas.AddElement(UI_TEXT, "MorbiusText_2");
	canvas["MorbiusText_2"]->SetPosition(1480.0, 660.0);
	canvas["MorbiusText_2"]->SetScale(400.0, 200.0);
	canvas["MorbiusText_2"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas["MorbiusText_2"]->SetBaseColour(1.0f, 0.85f, 0.0f, 1.0f);
	((Text*)canvas["MorbiusText_2"])->SetText("2023");
	((Text*)canvas["MorbiusText_2"])->SetTextAlignment(TEXT_RIGHT, TEXT_MIDDLE);

	canvas.AddElement(UI_TEXT, "StartText");
	canvas["StartText"]->SetPosition(50.0, 400.0);
	canvas["StartText"]->SetScale(480.0, 120.0);
	canvas["StartText"]->SetInteractable(true);
	canvas["StartText"]->SetHoverColour(0.0f, 1.0f, 0.0f);
	((Text*)canvas["StartText"])->SetText("Start");
	((Text*)canvas["StartText"])->SetTextAlignment(TEXT_CENTERED,TEXT_MIDDLE);

	canvas.AddElement(UI_TEXT, "QuitText");
	canvas["QuitText"]->SetPosition(50.0, 200.0);
	canvas["QuitText"]->SetScale(480.0, 120.0);
	canvas["QuitText"]->SetInteractable(true);
	canvas["QuitText"]->SetHoverColour(1.0f, 0.0f, 0.0f);
	((Text*)canvas["QuitText"])->SetText("Flee");
	((Text*)canvas["QuitText"])->SetTextAlignment(TEXT_CENTERED,TEXT_MIDDLE);

	canvas.AddElement(UI_TEXT, "Trading");
	canvas["Trading"]->SetPosition(515.0, 5.0);
	canvas["Trading"]->SetScale(1400.0, 40.0);
	canvas["Trading"]->SetStretch(UI_FIXED_ASPECT_X);
	((Text*)canvas["Trading"])->SetText("copyrighted under the licence of Morbisoft Corporation (pls dont sue me marvel)");
	((Text*)canvas["Trading"])->SetTextAlignment(TEXT_RIGHT, TEXT_MIDDLE);

	canvas.SetScreenSize(viewportWidth, viewportHeight);

#pragma endregion

	glClearColor(0.0, 1.0, 0.0, 1.0);
	while (!glfwWindowShouldClose(window)&&!hasStarted&&!hasQuit)
	{
		//printf("macska\n");
		glClear(GL_COLOR_BUFFER_BIT);

		ButtonHandler::UpdateStates(window);

		if (viewportChanged)
		{
			viewportChanged = false;
			canvas.SetScreenSize(viewportWidth, viewportHeight);
		}
		canvas.Render(cursorPos[0], cursorPos[1], ButtonHandler::GetKeyDown(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKey(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKeyUp(Buttons::BUTTON_MOUSE_LEFT));
	  
		glfwSwapBuffers(window);
		//glfwPollEvents();
	}
	hasStarted = false;

	TextureImporter::DestroyTexture(morbius);
	TextureImporter::DestroyTexture(uiTexture);

	Text::DestroyTTF();
}

void realgame(GLFWwindow* window)
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);

	gameQuit = false;

	SetMouseCallback(window, true, true);
	glfwSetScrollCallback(window, scrollCallback);
	//glfwSetCursorPosCallback(window, mouseCallback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	void defaultGlobalVariables_Game();
	ButtonHandler::UpdateStates(window);
	void defaultGlobalVariables_Game();

	//audio
	AudioManager audio;
	audio.PlaySFX(AM_MUSIC_5, true);

	Sound wasser=audio.PlaySFX(AM_WATER, false);
	wasser.Stop();

	//camera
	Camera cam;
	glm::vec3 camPos = glm::vec3(0.0f);
	glm::vec3 camFront = glm::vec3(0.0f);
	glm::vec3 camVelocity = glm::vec3(0.0f);
	cam_voidp = (void*)(&cam);
	cam.SetPosition(0.25f, 90.0f, -0.25f);

#pragma region CUBESHADER_SETUP

	unsigned int cubeshader = createShader(SHADER_CUBE);
	unsigned int watershader = createShader(SHADER_WATER);

	loadedBlockTextures[0]= TextureImporter::CreateTexture("_sprites/block_textures_13_simple.png", true);
	loadedBlockTextures[1] = TextureImporter::CreateTexture("_sprites/block_textures_13.png", true);
	loadedBlockTextures[2] = TextureImporter::CreateTexture("_sprites/block_textures_13_subtitles.png", true);
	loadedBlockTextures[3] = TextureImporter::CreateTexture("_sprites/block_textures_13_borsod.png", true);
	blockTexture = loadedBlockTextures[0];

	/*glActiveTexture(GL_TEXTURE1);
	unsigned int specularMap[SPECULAR_ANIMATION_LENGTH];
	specularMap[0] = TextureImporter::CreateTexture("_sprites/block_specular_maps_animated_1.png", true);
	specularMap[1] = TextureImporter::CreateTexture("_sprites/block_specular_maps_animated_2.png", true);
	specularMap[2] = TextureImporter::CreateTexture("_sprites/block_specular_maps_animated_3.png", true);
	specularMap[3] = TextureImporter::CreateTexture("_sprites/block_specular_maps_animated_4.png", true);
	specularMap[4] = TextureImporter::CreateTexture("_sprites/block_specular_maps_animated_5.png", true);
	specularMap[5] = TextureImporter::CreateTexture("_sprites/block_specular_maps_animated_6.png", true);
	glBindTexture(GL_TEXTURE_2D, specularMap[currentSpecularMap]);*/

  

	ChunkManager* cm = new ChunkManager(RENDER_DISTANCE, rand());
	glUseProgram(cubeshader);
	glUniform1f(glGetUniformLocation(cubeshader, "renderEnd"), RENDER_DISTANCE * 16.0f);
	glUniform1f(glGetUniformLocation(cubeshader, "renderEnd_water"), 10.0f);
	glUniform1i(glGetUniformLocation(cubeshader, "atlas"), 0);//texture beallitasa


	glUseProgram(watershader);
	glUniform1f(glGetUniformLocation(watershader, "renderEnd"), RENDER_DISTANCE * 16.0f);
	glUniform1f(glGetUniformLocation(watershader, "renderEnd_water"), 10.0f);
	glUniform1i(glGetUniformLocation(watershader, "atlas"), 0);//texture beallitasa
	glUniform1i(glGetUniformLocation(watershader, "specularAtlas"), 1);//specular map beallitasa
	glUniform3f(glGetUniformLocation(watershader, "lightPos"), 100.0f, 80.0f, 100.0f);
	glUniform1f(glGetUniformLocation(watershader, "shininess"), 16.0f);

#pragma endregion

	//selection

	SelectionRenderer* sr = new SelectionRenderer();
	int srPos[3];
	int raycastReturn;//whatever number the raycast returns


	//sky

	GLuint suntexture= TextureImporter::CreateTexture("_sprites/sun7.png", true);
	Sky* sky=new Sky();
	SkyElement sun(10.0, 40.0, -45.0, suntexture);
	(*sky) += sun;


	//sprite and bullets
	Sprite::InitializeSpriteRenderer();

	unsigned int bulletTexture = TextureImporter::CreateTexture("_sprites/bullet.png", true);
	Billboard bulletSprite;
	bulletSprite.SetTexture(bulletTexture);
	bulletSprite.SetPosition(glm::vec3(0.0, 100.0, 0.0));
	bulletSprite.SetTwoSided(true);

	BulletContainer bc;

	//hand
	Hand hand;
	hand.SetHeldBlock((enum blocks)hotbar[currentBlock]);
	hand.SetOffset(-0.1, -0.08, 0.11);


	//model
	unsigned int modelshader = createShader(SHADER_MODEL);
	glUseProgram(modelshader);
	glUniform1i(glGetUniformLocation(modelshader, "texture_diffuse1"), 0);
	glUseProgram(0);

	Model mymodel("hans_get_ze/flammenwerfer.obj");
	mymodel.SetScale(glm::vec3(0.02f, 0.02f, 0.02f));
	mymodel.SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));




#pragma region UI_Setup

	viewportChanged = true;


	//canvas
	Text::ImportTTF("_fonts/Adore64.ttf");

	unsigned int uiTexture = TextureImporter::CreateTexture("_sprites/ui_textures_8.png", true);
	unsigned int gunTexture= TextureImporter::CreateTexture("_sprites/machinegun_hotbar.png", true);
	unsigned int inventoryTexture = TextureImporter::CreateTexture("_sprites/inventory_texture.png", true);
	
	
	Canvas canvas_ingame;
	//canvas_ingame.SetScreenSize(1920, 1080);//egyelore mindig ugy renderel, mintha 1920x1080-as ablakban jatszodna

	canvas_ingame.AddElement(UI_IMAGE, "cursor");
	canvas_ingame["cursor"]->SetPosition(945, 525.0);
	canvas_ingame["cursor"]->SetScale(30.0, 30.0);
	canvas_ingame["cursor"]->SetUVPosition(0.0, 0.8);
	canvas_ingame["cursor"]->SetUVScale(0.1, 0.1);
	((Image*)canvas_ingame["cursor"])->SetTexture(uiTexture);
 

	char name_temp[40];
	strcpy(name_temp, "slot0");
	for (unsigned int i = 0; i < 9; i++)
	{
		name_temp[4] = name_temp[4] + 1;
		canvas_ingame.AddElement(UI_IMAGE, name_temp);

		canvas_ingame[name_temp]->SetPosition(518.0+i*100.0, 6.0);
		canvas_ingame[name_temp]->SetScale(88.0f, 88.0f);

		((Image*)canvas_ingame[name_temp])->SetTexture(blockTexture);

		canvas_ingame[name_temp]->SetAnchor(UI_BOTTOM_CENTER);
		canvas_ingame[name_temp]->SetStretch(UI_FIXED_ASPECT_X);
	}
	RefreshHotbar(canvas_ingame);

	canvas_ingame.AddElement(UI_IMAGE, "hotbar");
	canvas_ingame["hotbar"]->SetPosition(512, 0.0);
	canvas_ingame["hotbar"]->SetScale(900.0, 100.0);
	canvas_ingame["hotbar"]->SetUVPosition(0.0, 0.9);
	canvas_ingame["hotbar"]->SetUVScale(0.9, 0.1);
	canvas_ingame["hotbar"]->SetBaseColour(0.5f, 0.5f, 0.5f, 1.0f);
	canvas_ingame["hotbar"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_ingame["hotbar"]->SetStretch(UI_FIXED_ASPECT_X);
	((Image*)canvas_ingame["hotbar"])->SetTexture(uiTexture);

	canvas_ingame.AddElement(UI_IMAGE, "hotbar_selection");
	canvas_ingame["hotbar_selection"]->SetPosition(512, 0.0);
	canvas_ingame["hotbar_selection"]->SetScale(100.0, 100.0);
	canvas_ingame["hotbar_selection"]->SetUVPosition(0.9, 0.9);
	canvas_ingame["hotbar_selection"]->SetUVScale(0.1, 0.1);
	canvas_ingame["hotbar_selection"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_ingame["hotbar_selection"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas_ingame["hotbar_selection"]->SetBaseColour(1.0f, 0.0f, 0.0f, 1.0f);
	((Image*)canvas_ingame["hotbar_selection"])->SetTexture(uiTexture);
	
	canvas_ingame.AddElement(UI_TEXT, "FPS");
	canvas_ingame["FPS"]->SetPosition(1700.0, 1020.0);
	canvas_ingame["FPS"]->SetScale(200.0, 50.0);
	canvas_ingame["FPS"]->SetAnchor(UI_BOTTOM_RIGHT);
	canvas_ingame["FPS"]->SetStretch(UI_FIXED_ASPECT_X);
	((Text*)canvas_ingame["FPS"])->SetText("FPS: 0");
	((Text*)canvas_ingame["FPS"])->SetTextAlignment(TEXT_RIGHT, TEXT_TOP);

	canvas_ingame.AddElement(UI_TEXT, "CPS");
	canvas_ingame["CPS"]->SetPosition(1700.0, 965.0);
	canvas_ingame["CPS"]->SetScale(200.0, 50.0);
	canvas_ingame["CPS"]->SetAnchor(UI_BOTTOM_RIGHT);
	canvas_ingame["CPS"]->SetStretch(UI_FIXED_ASPECT_X);
	((Text*)canvas_ingame["CPS"])->SetText("CPS: 0");
	((Text*)canvas_ingame["CPS"])->SetTextAlignment(TEXT_RIGHT, TEXT_TOP);

	canvas_ingame.AddElement(UI_TEXT, "X");
	canvas_ingame["X"]->SetPosition(20.0, 1020.0);
	canvas_ingame["X"]->SetScale(200.0, 50.0);
	canvas_ingame["X"]->SetAnchor(UI_BOTTOM_LEFT);
	canvas_ingame["X"]->SetStretch(UI_FIXED_ASPECT_X);
	((Text*)canvas_ingame["X"])->SetText("X: 0.0");
	((Text*)canvas_ingame["X"])->SetTextAlignment(TEXT_LEFT, TEXT_TOP);

	canvas_ingame.AddElement(UI_TEXT, "Y");
	canvas_ingame["Y"]->SetPosition(20.0,970.0);
	canvas_ingame["Y"]->SetScale(200.0, 50.0);
	canvas_ingame["Y"]->SetAnchor(UI_BOTTOM_LEFT);
	canvas_ingame["Y"]->SetStretch(UI_FIXED_ASPECT_X);
	((Text*)canvas_ingame["Y"])->SetText("Y: 0.0");
	((Text*)canvas_ingame["Y"])->SetTextAlignment(TEXT_LEFT, TEXT_TOP);

	canvas_ingame.AddElement(UI_TEXT, "Z");
	canvas_ingame["Z"]->SetPosition(20.0, 920.0);
	canvas_ingame["Z"]->SetScale(200.0, 50.0);
	canvas_ingame["Z"]->SetAnchor(UI_BOTTOM_LEFT);
	canvas_ingame["Z"]->SetStretch(UI_FIXED_ASPECT_X);
	((Text*)canvas_ingame["Z"])->SetText("Z: 0.0");
	((Text*)canvas_ingame["Z"])->SetTextAlignment(TEXT_LEFT, TEXT_TOP);


	canvas_ingame.AddElement(UI_TEXT, "gepfegyver_activation");
	canvas_ingame["gepfegyver_activation"]->SetPosition(20.0, 20.0);
	canvas_ingame["gepfegyver_activation"]->SetScale(300.0, 75.0);
	canvas_ingame["gepfegyver_activation"]->SetAnchor(UI_BOTTOM_LEFT);
	canvas_ingame["gepfegyver_activation"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas_ingame["gepfegyver_activation"]->SetBaseColour(1.0, 0.0, 0.0, 1.0);
	((Text*)canvas_ingame["gepfegyver_activation"])->SetText("");
	((Text*)canvas_ingame["gepfegyver_activation"])->SetTextAlignment(TEXT_LEFT, TEXT_TOP);


	canvas_ingame.SetScreenSize(viewportWidth, viewportHeight);//a gombok hozzaadasa utan allitom be, hogy egybol at is allitsa a mereteket, ha kell



	Canvas canvas_pause;
	//canvas_pause.SetVisibility(false);//eredetileg ki van kapcsolva

	canvas_pause.AddElement(UI_IMAGE, "background");
	canvas_pause["background"]->SetPosition(0.0f,0.0f);//560.0, 240.0
	canvas_pause["background"]->SetScale(1920.0f,1080.0f);//800.0, 600.0
	canvas_pause["background"]->SetUVPosition(0.8, 0.8);
	canvas_pause["background"]->SetUVScale(0.1, 0.1);
	canvas_pause["background"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_pause["background"]->SetStretch(UI_STRETCH);
	canvas_pause["background"]->SetBaseColour(0.0f, 0.0f, 0.0f,0.5f);
	((Image*)canvas_pause["background"])->SetTexture(uiTexture);


	canvas_pause.AddElement(UI_TEXT, "Title");
	canvas_pause["Title"]->SetPosition(660.0, 640.0);
	canvas_pause["Title"]->SetScale(600.0, 200.0);
	canvas_pause["Title"]->SetBaseColour(1.0, 0.850, 0.0, 1.0);
	canvas_pause["Title"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_pause["Title"]->SetStretch(UI_FIXED_ASPECT_X);
	(dynamic_cast<Text*>(canvas_pause["Title"]))->SetText("PAUSED");
	(dynamic_cast<Text*>(canvas_pause["Title"]))->SetTextAlignment(TEXT_CENTERED, TEXT_MIDDLE);


	canvas_pause.AddElement(UI_TEXT, "StartButton");
	canvas_pause["StartButton"]->SetPosition(810.0, 440.0);
	canvas_pause["StartButton"]->SetScale(300.0, 100.0);
	canvas_pause["StartButton"]->SetBaseColour(0.0, 1.0, 0.0, 1.0);
	canvas_pause["StartButton"]->SetHoverColour(0.0, 0.5, 0.0, 1.0);
	(dynamic_cast<Text*>(canvas_pause["StartButton"]))->SetText("I AM BASED");
	(dynamic_cast<Text*>(canvas_pause["StartButton"]))->SetTextAlignment(TEXT_CENTERED, TEXT_MIDDLE);
	canvas_pause["StartButton"]->SetInteractable(true);
	canvas_pause["StartButton"]->SetCallback(INTERACTION::INTERACTION_UP,startButton_game, (void*)window);
	

	canvas_pause.AddElement(UI_TEXT, "QuitButton");
	canvas_pause["QuitButton"]->SetPosition(810.0, 290.0);
	canvas_pause["QuitButton"]->SetScale(300.0, 100.0);
	canvas_pause["QuitButton"]->SetBaseColour(1.0,0.0,0.0,1.0);
	canvas_pause["QuitButton"]->SetHoverColour(0.5, 0.0, 0.0, 1.0);
	(dynamic_cast<Text*>(canvas_pause["QuitButton"]))->SetText("I AM WEAK");
	(dynamic_cast<Text*>(canvas_pause["QuitButton"]))->SetTextAlignment(TEXT_CENTERED, TEXT_MIDDLE);
	canvas_pause["QuitButton"]->SetInteractable(true);
	canvas_pause["QuitButton"]->SetCallback(INTERACTION::INTERACTION_UP,quitButton_game, (void*)window);


	//fontos, hogy ez a ui setup vegen legyen
	canvas_pause.SetScreenSize(viewportWidth, viewportHeight);//a gombok hozzaadasa utan allitom be, hogy egybol at is allitsa a mereteket, ha kell

	

	Canvas canvas_inventory;

	canvas_inventory.AddElement(UI_IMAGE, "background");
	canvas_inventory["background"]->SetPosition(528.0f, 396.0f);
	canvas_inventory["background"]->SetScale(864.0f, 288.0f);
	canvas_inventory["background"]->SetUVPosition(0.8, 0.8);
	canvas_inventory["background"]->SetUVScale(0.1, 0.1);
	canvas_inventory["background"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_inventory["background"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas_inventory["background"]->SetBaseColour(0.0f, 0.0f, 0.0f, 0.5f);
	if (dynamic_cast<Image*>(canvas_inventory["background"]) != 0) dynamic_cast<Image*>(canvas_inventory["background"])->SetTexture(uiTexture);

	canvas_inventory.AddElement(UI_IMAGE, "inventory_background");
	canvas_inventory["inventory_background"]->SetPosition(528.0f, 396.0f);
	canvas_inventory["inventory_background"]->SetScale(864.0f, 288.0f);
	canvas_inventory["inventory_background"]->SetUVPosition(0.0, 0.0);
	canvas_inventory["inventory_background"]->SetUVScale(1.0, 1.0);
	canvas_inventory["inventory_background"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_inventory["inventory_background"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas_inventory["inventory_background"]->SetBaseColour(0.5f, 0.5f, 0.5f, 1.0f);
	if(dynamic_cast<Image*>(canvas_inventory["inventory_background"])!=0) dynamic_cast<Image*>(canvas_inventory["inventory_background"])->SetTexture(inventoryTexture);

	//filling the slots
	strcpy(name_temp, "inventory_slot0");//korabban hasznalt, 40 byte hosszu name_temp tomb

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			name_temp[14] += 1;//inventory_slot1-tol inventory_slot27-ig

			canvas_inventory.AddElement(UI_IMAGE, name_temp);

			canvas_inventory[name_temp]->SetPosition(534.0 + j * 96.0, 595.0-i*96.0);
			canvas_inventory[name_temp]->SetScale(86.0f, 86.0f);


			float _pos[2];
			BlockDatabase::GetUv(0, 0, inventory[i*9+j], _pos[0], _pos[1]);
			canvas_inventory[name_temp]->SetUVPosition(_pos[0], _pos[1]);

			if (inventory[i * 9 + j] == BORSOD)
				canvas_inventory[name_temp]->SetUVScale(0.4f, 0.4f);
			else
				canvas_inventory[name_temp]->SetUVScale(0.1f, 0.1f);

			dynamic_cast<Image*>(canvas_inventory[name_temp])->SetTexture(blockTexture);


			canvas_inventory[name_temp]->SetAnchor(UI_BOTTOM_CENTER);
			canvas_inventory[name_temp]->SetStretch(UI_FIXED_ASPECT_X);

			if (inventory[i * 9 + j] != AIR)
			{
				canvas_inventory[name_temp]->SetInteractable(true);
				canvas_inventory[name_temp]->SetCallback(INTERACTION::INTERACTION_DOWN, inventorySlot, (void*)inventory[i * 9 + j]);
			}
		}
	}


	canvas_inventory.AddElement(UI_IMAGE, "hotbar");
	canvas_inventory["hotbar"]->SetPosition(528.0, 300.0);
	canvas_inventory["hotbar"]->SetScale(864.0, 96.0);
	canvas_inventory["hotbar"]->SetUVPosition(0.0, 0.9);
	canvas_inventory["hotbar"]->SetUVScale(0.9, 0.1);
	canvas_inventory["hotbar"]->SetBaseColour(1.0f, 0.0f, 0.0f, 1.0f);
	canvas_inventory["hotbar"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_inventory["hotbar"]->SetStretch(UI_FIXED_ASPECT_X);
	((Image*)canvas_inventory["hotbar"])->SetTexture(uiTexture);

	strcpy(name_temp, "slot0");
	for (int i = 0; i < 9; i++)
	{
		name_temp[4] = name_temp[4] + 1;
		canvas_inventory.AddElement(UI_IMAGE, name_temp);

		canvas_inventory[name_temp]->SetPosition(534.0 + i * 96.0, 306.0);
		canvas_inventory[name_temp]->SetScale(84.0f, 84.0f);

		((Image*)canvas_inventory[name_temp])->SetTexture(blockTexture);

		canvas_inventory[name_temp]->SetAnchor(UI_BOTTOM_CENTER);
		canvas_inventory[name_temp]->SetStretch(UI_FIXED_ASPECT_X);

		canvas_inventory[name_temp]->SetInteractable(true);
		canvas_inventory[name_temp]->SetCallback(INTERACTION::INTERACTION_UP, inventoryHotbarSlot, (void*)i);
	}
	RefreshHotbar(canvas_inventory);


	canvas_inventory.AddElement(UI_IMAGE, "draggedSlot");
	canvas_inventory["draggedSlot"]->SetScale(96.0f, 96.0f);
	canvas_inventory["draggedSlot"]->SetAnchor(UI_BOTTOM_CENTER);
	canvas_inventory["draggedSlot"]->SetStretch(UI_FIXED_ASPECT_X);
	canvas_inventory["draggedSlot"]->SetInteractable(false);
	canvas_inventory["draggedSlot"]->SetVisibility(false);
	((Image*)canvas_inventory["draggedSlot"])->SetTexture(blockTexture);


	canvas_inventory.SetScreenSize(viewportWidth, viewportHeight);

#pragma endregion

	//it's morbin' time

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK,polygonMode);
	glLineWidth(3.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	lastFrame = glfwGetTime();
	lastSecond = lastFrame;

	lastFixedUpdate = lastFrame;
	
	while (!gameQuit&&!glfwWindowShouldClose(window))
	{
		//time things
		{
			currentTime = glfwGetTime();

			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;
			//if (deltaTime > 0.02f) printf("length of the frame: %.0f ms\n", deltaTime * 1000.0f);

			if (currentTime - lastSecond >= 1.0f)
			{
				//GLint total_mem_kb = 0;
				//glGetIntegerv(0x9048, &total_mem_kb);

				//GLint cur_avail_mem_kb = 0;
				//glGetIntegerv(0x9049, &cur_avail_mem_kb);

				lastSecond = currentTime;
				printf("fps: %d\n", frameCount);
				//printf("location: x: %.2f | y: %.2f | z: %.2f\n\n", camPos.x, camPos.y, camPos.z);
				//printf("total memory: %d MB, currently available: %d MB\n\n", total_mem_kb / 1024, cur_avail_mem_kb / 1024);
				lastFrameCount = frameCount;
				frameCount = 0;
			
				lastCPS = CPS;
				CPS = 0;

				randomFrameCount = (rand() % 900)+100;
			}
			frameCount++;
		}

		cam.CalculateVectors();
		ButtonHandler::UpdateStates(window);
		if (currentmenu == MENU_INGAME)
		{
			processInput_NonPhysics_ingame();
			inGepfegyverChanged=processGepfegyverActivationInput(gepfegyverActivationState, inGepfegyver);
		}
		processInput_MenuChange(window);

		camPos = cam.GetPosition();
		camFront = cam.GetFront();

		//physics
		if (currentTime - lastFixedUpdate > PHYSICS_TIME_STEP&&currentmenu==MENU_INGAME)
		{
			fixedDeltaTime = currentTime - lastFixedUpdate;
			lastFixedUpdate = currentTime;

			//collisionType = Physics::COLLISION_NONE;

			//mig az isCameraInWater fontos, hogy a physicsupdate utan legyen kiszamolva, ezeknek tobb ertelme van, ha az update elott vannak kiszamolva
			isPlayerInWater[0] = cm->isPointSubmerged(glm::vec3(camPos.x, camPos.y - 1.4f, camPos.z));
			isPlayerInWater[1] = cm->isPointSubmerged(glm::vec3(camPos.x, camPos.y - 0.6f, camPos.z));

			camVelocity = processMovementInput(window, cam);
			camVelocity.y = camVelocity.y > 20.0f ? camVelocity.y = 20.0f : camVelocity.y;

			//sound effects
			if (godModeAborted) { godModeAborted = false; audio.PlaySFX(AM_GODMODE_ABORT); jumpStarted = false; }//azert kell a jumpstartedot valtoztatni, bentmaradhat a morbin time utan egy jumpStarted==true
			if (jumpStarted) { jumpStarted = false; audio.PlaySFX(AM_JUMP); }
			if (godModeInitiated) { godModeInitiated = false; audio.PlaySFX(AM_GODMODE_INITIATE); }


			//checking for collisions
			if (!inGodMode)
			{
				if (fixedDeltaTime < 0.1f)
				{
					camPos = cam.GetPosition() + (float)fixedDeltaTime * camVelocity;
					collisionType=cm->PhysicsUpdate(camPos, camVelocity);

					cam.SetPosition(camPos);
					cam.SetVelocity(camVelocity);
				}
				else std::cout << "physics update ignored" << '\n';
			}
			else
			{ 
				camPos = cam.GetPosition() + (float)fixedDeltaTime * camVelocity;

				cam.SetPosition(camPos);
				cam.SetVelocity(camVelocity);
			}

			isCameraInWater = cm->isPointSubmerged(camPos);

			//animation
			/*if ((++updatesSinceSpecularChange) >= SPECULAR_ANIMATION_SPEED)
			{
				updatesSinceSpecularChange = 0;

				currentSpecularMap = currentSpecularMap >= SPECULAR_ANIMATION_LENGTH ? currentSpecularMap = 0 : currentSpecularMap + 1;
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularMap[currentSpecularMap]);
			}*/


			//ha a jatekos a vizben van es a hang megallt volna
			if (isPlayerInWater[0] && wasser.isFinished())
				wasser.Replay();


			bc.StepBullets(cm);
			//continue;
		}

		//calculate FOV (vmi okos embertol kolcsonvett keplet az fps-fuggetlen linearis interpolaciora)
		if (inSprint)
		{
			if (inZoom) currentFOV = currentFOV + (ZOOM_FOV * SPRINT_FOV_MULTIPLIER - currentFOV) * (1.0f - powf(2.71828f, -10.0f * deltaTime));
			else currentFOV = currentFOV + (NORMAL_FOV * SPRINT_FOV_MULTIPLIER - currentFOV) * (1.0f - powf(2.71828f, -10.0f * deltaTime));
		}
		else
		{
			if (inZoom) currentFOV = currentFOV + (ZOOM_FOV - currentFOV) * (1.0f - powf(2.71828f, -10.0f * deltaTime));
			else currentFOV = currentFOV + (NORMAL_FOV - currentFOV) * (1.0f - powf(2.71828f, -10.0f * deltaTime));
		}


		//rendering
		glClearDepth(1.0);
		if (isCameraInWater) glClearColor(deepWater.x, deepWater.y, deepWater.z, deepWater.w);
		else glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
		glm::mat4 view = cam.GetViewMatrix();
		glm::mat4 projection = Camera::GetProjectionMatrix(false, glm::radians(currentFOV), aspectRatio, CLIP_DISTANCE_NEAR, CLIP_DISTANCE_FAR);

#pragma region shader_setup

		glUseProgram(cubeshader);
		
		glUniformMatrix4fv(glGetUniformLocation(cubeshader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(cubeshader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cubeshader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(cubeshader, "cameraPos"), camPos.x, camPos.y, camPos.z);

		glUniform4f(glGetUniformLocation(cubeshader, "clearColor"), clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glUniform4f(glGetUniformLocation(cubeshader, "waterColor"), deepWater.x, deepWater.y, deepWater.z, deepWater.w);

		if (isCameraInWater)
		{
			glUniform1i(glGetUniformLocation(cubeshader, "isCameraInWater"), 1);//vertex shader
			glUniform1i(glGetUniformLocation(cubeshader, "isCamInWater"), 1);//fragment shader
		}
		else
		{
			glUniform1i(glGetUniformLocation(cubeshader, "isCameraInWater"), 0);//vertex shader
			glUniform1i(glGetUniformLocation(cubeshader, "isCamInWater"), 0);//fragment shader
		}

		glUseProgram(watershader);

		/*glUniformMatrix4fv(glGetUniformLocation(watershader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(watershader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(watershader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));*/

		glm::mat4 combined = projection * view * model;
		//glUniformMatrix4fv(glGetUniformLocation(watershader, "combined_vs"), 1, GL_FALSE, glm::value_ptr(combined));
		glUniformMatrix4fv(glGetUniformLocation(watershader, "combined_gs"), 1, GL_FALSE, glm::value_ptr(combined));

		glUniform1f(glGetUniformLocation(watershader, "time"), currentTime);//geometry shader

		glUniform3f(glGetUniformLocation(watershader, "cameraPos"), camPos.x, camPos.y, camPos.z);
		glUniform3f(glGetUniformLocation(watershader, "camPos"), camPos.x, camPos.y, camPos.z);//ez a fragmentshaderes camera position

		glUniform4f(glGetUniformLocation(watershader, "clearColor"), clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glUniform4f(glGetUniformLocation(watershader, "waterColor"), deepWater.x, deepWater.y, deepWater.z, deepWater.w);

		if (isCameraInWater)
		{
			glUniform1i(glGetUniformLocation(watershader, "isCameraInWater"), 1);//vertex shader
			glUniform1i(glGetUniformLocation(watershader, "isCamInWater"), 1);//fragment shader
		}
		else
		{
			glUniform1i(glGetUniformLocation(watershader, "isCameraInWater"), 0);//vertex shader
			glUniform1i(glGetUniformLocation(watershader, "isCamInWater"), 0);//fragment shader
		}

		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blockTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);*/

#pragma endregion    


		//raycast and block placement
		if (inGepfegyver == false)//azaz normalisan tudja utni es lerakni a blokkokat
		{
			if ((raycastReturn = cm->Raycast2(camPos, camFront, 5.0f, srPos[0], srPos[1], srPos[2], false, false)))
				sr->Render(glm::vec3((float)srPos[0], (float)srPos[1], (float)srPos[2]), projection * view);
			if (leftPressed && raycastReturn && currentTime - BLOCK_PLACEMENT_COOLDOWN > lastBlockBreak)
			{
				if (srPos[1] != 0) cm->AddBlock(srPos[0], srPos[1], srPos[2], AIR);
				lastBlockBreak = currentTime;
				//hasBroken = true;
				audio.PlaySFX(AM_BLOCK_BREAK);
				//std::cout << "broken " <<srPos[0]<<" "<<srPos[1]<<" "<<srPos[2]<< std::endl;
			}
			else if (rightPressed && raycastReturn && currentTime - BLOCK_PLACEMENT_COOLDOWN > lastBlockPlacement)
			{
				lastBlockPlacement = currentTime;

				raycastReturn--;//hogy kettovel osztva jol kijojjenek az indexek
				if (raycastReturn % 2)//x-,y- vagy z-
					srPos[raycastReturn / 2]++;
				else
					srPos[raycastReturn / 2]--;

				if (srPos[1] < 149 && srPos[1]>0 && !Physics::isIntersecting(camPos, glm::vec3(srPos[0], srPos[1], srPos[2])))
				{
					cm->AddBlock(srPos[0], srPos[1], srPos[2], hotbar[currentBlock]);
					//hasPlaced = true;
					audio.PlaySFX(AM_BLOCK_PLACE);
				}
			}
		}
		else//gepfegyver jee
		{
			if (leftPressed)
			{
				if (GEPFEGYVER_COOLDOWN + lastGepfegyver < currentTime)//loves van
				{
					lastGepfegyver = currentTime;

					//if (cm->Raycast2_unprecise(camPos, camFront, RENDER_DISTANCE*16.0f, srPos[0], srPos[1], srPos[2], false))
						//cm->AddBlock(srPos[0], srPos[1], srPos[2], AIR);

					audio.PlaySFX(AM_GUNSHOT);

					//visszalokes
					cam.Rotate(((rand() / (float)RAND_MAX)) - 0.5f, 2.0f);

					bc.AddBullet(camPos-0.1f*cam.GetUp()+1.0f*camFront, camFront);
				}
			}
		}


		//sky
		if (!isCameraInWater)
		{
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, sun.GetTexture());
			glm::mat4 pv = projection * view;
			sky->RenderSky(pv, camPos);
		}
		


		//chunks

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blockTexture);
		//printf("%u\n",sky->elements[0].texture);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);


		cm->UnloadChunks(camPos.x, camPos.z);
		if (lastChunkLoad + ChunkManager::CHUNK_LOADING_SPEED < currentTime)
		{
			cm->LoadChunks(camPos.x, camPos.z);
			//std::async(std::launch::async, &ChunkManager::LoadChunks,cm,camPos.x, camPos.z);
			lastChunkLoad = currentTime;
		}
		cm->RecalculateChunks();
		//std::async(std::launch::async,&ChunkManager::RecalculateChunks,cm);


		glUseProgram(cubeshader);
		cm->RenderChunks(false);//chunkmesh

		glUseProgram(watershader);
		cm->RenderChunks(true);//watermesh

		//sprite
		bulletSprite.SetStareVictim(camPos);
		bc.RenderBullets(bulletSprite, view, projection);


		glClear(GL_DEPTH_BUFFER_BIT);//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//hand
		if (!inGepfegyver)
			hand.Render(blockTexture, aspectRatio);
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			//glDisable(GL_DEPTH_TEST);
 
			//ez csak azert kell, mert mivel a gepfegyver igazabol nincsen world space-ben forgatva, igy a fenyt kell forgatni hozza kepest
			glUseProgram(modelshader);            
			glUniform3f(glGetUniformLocation(modelshader, "lightDir"), -cos(glm::radians(-45.0f)-cam.GetYaw()), sin(glm::radians(40.0f) - cam.GetPitch()), sin(glm::radians(-45.0f) - cam.GetYaw()));//vmiert a GetPitch() es a GetYaw() az radiant ad vissza
			glUseProgram(0);


			glm::vec3 offset = hand.GetOffset();
			if (rightPressed)
			{
				offset += glm::vec3(0.1001f, 0.02215f, 0.0f);
				//offset += glm::vec3(0.0f, 0.02215f, 0.0f);
				mymodel.SetPosition(offset);
				mymodel.Draw(modelshader, glm::lookAt(glm::vec3(0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)), Camera::GetProjectionMatrix(false, glm::radians(currentFOV), aspectRatio, 0.05, 10.0));//a matrixok a kez alapjan lettek bemasolva
			}
			else
			{
				offset += glm::vec3(-0.15f, -0.05f, 0.0f);
				mymodel.SetPosition(offset);
				mymodel.Draw(modelshader, glm::lookAt(glm::vec3(0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)), Camera::GetProjectionMatrix(false, glm::radians(90.0), aspectRatio, 0.05, 10.0));//a matrixok a kez alapjan lettek bemasolva
			}
	
	   
			glEnable(GL_DEPTH_TEST);
			glCullFace(GL_FRONT);
		}


		//canvas
#pragma region UI_UPDATING

		if (currentTime > lastTextRefresh + TEXT_REFRESH_INTERVAL)//rendszeres ui frissites
		{
			lastTextRefresh = currentTime;

			char temp[40];


			sprintf_s(temp, "FPS: %d%d", lastFrameCount, randomFrameCount);
			((Text*)canvas_ingame["FPS"])->SetText(temp);

			sprintf_s(temp, "CPS: %d", lastCPS);
			((Text*)canvas_ingame["CPS"])->SetText(temp);


			sprintf_s(temp, "X: %.2f", camPos.x);
			((Text*)canvas_ingame["X"])->SetText(temp);
			sprintf_s(temp, "Y: %.2f", camPos.y);
			((Text*)canvas_ingame["Y"])->SetText(temp);
			sprintf_s(temp, "Z: %.2f", camPos.z);
			((Text*)canvas_ingame["Z"])->SetText(temp);


			strcpy_s(temp, 40, gepfegyverActivationText);
			temp[gepfegyverActivationState] = '\0';
			((Text*)canvas_ingame["gepfegyver_activation"])->SetText(temp);


			
			strcpy_s(temp, 40, "slot0");
			for (int i = 0; i < 9; i++)
			{
				temp[4]++;
				dynamic_cast<Image*>(canvas_inventory[temp])->SetTexture(blockTexture);
				dynamic_cast<Image*>(canvas_ingame[temp])->SetTexture(blockTexture);
			}
			strcpy_s(temp, 40, "inventory_slot0");
			for (int i = 0; i < 27; i++)
			{
				temp[14]++;
				dynamic_cast<Image*>(canvas_inventory[temp])->SetTexture(blockTexture);
			}
		}
		if (viewportChanged)//kepernyomeret valtasnal ujra kell szamoni a uielement-ek helyet
		{
			viewportChanged = false;



			float defaultAspectRatio = canvas_inventory.GetDefaultScreenSize(0) / canvas_inventory.GetDefaultScreenSize(1);
			char name_temp2[40];
			float _pos2[2];
			float _newY;
			float _scale;


			strcpy(name_temp2, "inventory_slot0");
			_newY = 498.0f + 96.0 * (aspectRatio / defaultAspectRatio);
			for (unsigned int i = 0; i < 9; i++)
			{
				name_temp2[14]++;
				canvas_inventory[name_temp2]->GetPosition(_pos2[0], _pos2[1]);
				_pos2[1] = _newY;
				canvas_inventory[name_temp2]->SetPosition(_pos2[0], _pos2[1]);
			}
			name_temp2[14] = '0' + 18;
			_newY=498.0f - 96.0 * (aspectRatio / defaultAspectRatio);
			for (unsigned int i = 0; i < 9; i++)
			{
				name_temp2[14]++;
				canvas_inventory[name_temp2]->GetPosition(_pos2[0], _pos2[1]);
				_pos2[1] = _newY;
				canvas_inventory[name_temp2]->SetPosition(_pos2[0], _pos2[1]);
			}

			canvas_inventory["hotbar"]->GetPosition(_pos2[0], _pos2[1]);
			_newY -= 101.0f * (aspectRatio / defaultAspectRatio);
			canvas_inventory["hotbar"]->SetPosition(_pos2[0], _newY);
			_newY += 5.0f * (aspectRatio / defaultAspectRatio);
			strcpy_s(name_temp2, 40, "slot0");
			for (unsigned int i = 0; i < 9; i++)
			{
				name_temp2[4]++;
				canvas_inventory[name_temp2]->GetPosition(_pos2[0], _pos2[1]);
				_pos2[1] = _newY;
				canvas_inventory[name_temp2]->SetPosition(_pos2[0], _pos2[1]);
			}


			canvas_ingame.SetScreenSize(viewportWidth, viewportHeight);
			canvas_pause.SetScreenSize(viewportWidth, viewportHeight);
			canvas_inventory.SetScreenSize(viewportWidth, viewportHeight);
		}
		if (slotChanged)//ha blokkot valt, akkor a hotbaron es a kezen is kell valtoztatni
		{
			canvas_ingame["hotbar_selection"]->SetPosition(512.0 + currentBlock * 100.0f, 0.0f);

			hand.SetHeldBlock((enum blocks)hotbar[currentBlock]);
			slotChanged = false;

			canvas_ingame.SetScreenSize(viewportWidth, viewportHeight);

			/*if (hand.GetHeldBlock() == AIR)	inGepfegyver=true;
			else    inGepfegyver = false;

			canvas_ingame["cursor"]->SetVisibility(!inGepfegyver);*/
		}
		if (hotbarContentChanged)
		{
			RefreshHotbar(canvas_ingame);
			RefreshHotbar(canvas_inventory);
			hand.SetHeldBlock((enum blocks)hotbar[currentBlock]);

			hotbarContentChanged = false;
		}
		if (inGepfegyverChanged)//ha valt a gepfegyver es a blokkok kozott (beirja, hogy MORBIUS)
		{
			canvas_ingame["cursor"]->SetVisibility(!inGepfegyver);
			canvas_ingame["hotbar"]->SetVisibility(!inGepfegyver);
			canvas_ingame["hotbar_selection"]->SetVisibility(!inGepfegyver);

			char name_temp[40];
			strcpy(name_temp, "slot0");
			for(name_temp[4]='1';name_temp[4]<='9';name_temp[4]++)
				canvas_ingame[name_temp]->SetVisibility(!inGepfegyver);

			inGepfegyverChanged = false;
		}
		if (inDrag)
		{
			if (dragStarted)
			{
				float _pos2[2];
				BlockDatabase::GetUv(0, 0, draggedItem, _pos2[0], _pos2[1]);
				canvas_inventory["draggedSlot"]->SetUVPosition(_pos2[0], _pos2[1]);

				if (draggedItem == BORSOD)
					canvas_inventory["draggedSlot"]->SetUVScale(0.4f, 0.4f);
				else
					canvas_inventory["draggedSlot"]->SetUVScale(0.1f, 0.1f);

				canvas_inventory["draggedSlot"]->SetVisibility(true);

				dragStarted = false;
			}
			float defaultAspectRatio = canvas_inventory.GetDefaultScreenSize(0) / canvas_inventory.GetDefaultScreenSize(1);
			canvas_inventory["draggedSlot"]->SetPosition(cursorPos[0]-48.0f, -48.0f+cursorPos[1] * aspectRatio / defaultAspectRatio);
			
			canvas_inventory.SetScreenSize(viewportWidth, viewportHeight);
		}
#pragma endregion

		switch (currentmenu)
		{
		case MENU_INGAME:
			canvas_ingame.SetVisibility(true);
			canvas_pause.SetVisibility(false);
			canvas_inventory.SetVisibility(false);
			break;

		case MENU_PAUSE:
			canvas_ingame.SetVisibility(false);
			canvas_pause.SetVisibility(true);
			canvas_inventory.SetVisibility(false);
			break;

		case MENU_INVENTORY:
			canvas_ingame.SetVisibility(false);
			canvas_pause.SetVisibility(false);
			canvas_inventory.SetVisibility(true);
			break;
		}
		canvas_ingame.Render(cursorPos[0],cursorPos[1], ButtonHandler::GetKeyDown(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKey(Buttons::BUTTON_MOUSE_LEFT),ButtonHandler::GetKeyUp(Buttons::BUTTON_MOUSE_LEFT));
		canvas_pause.Render(cursorPos[0], cursorPos[1], ButtonHandler::GetKeyDown(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKey(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKeyUp(Buttons::BUTTON_MOUSE_LEFT));
		canvas_inventory.Render(cursorPos[0], cursorPos[1], ButtonHandler::GetKeyDown(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKey(Buttons::BUTTON_MOUSE_LEFT), ButtonHandler::GetKeyUp(Buttons::BUTTON_MOUSE_LEFT));
		
		//ha inventory huzasnal mashol eresztene fel a gombnyomast, akkor torolje
		if (ButtonHandler::GetKeyUp(Buttons::BUTTON_MOUSE_LEFT))
		{
			draggedItem = 0;
			inDrag = false;
			canvas_inventory["draggedSlot"]->SetVisibility(false);
		}

		glfwSwapBuffers(window);
		//glfwPollEvents();


		/*double timeLeft = glfwGetTime() - currentTime;
		timeLeft = (0.003) - timeLeft;

		if (timeLeft > 0.0)
			std::this_thread::sleep_for(std::chrono::milliseconds(((long)(timeLeft * 1000.0))));*/
	}

	delete cm;
	delete sr;
	delete sky;
	destroyShader(cubeshader);
	destroyShader(watershader);
	destroyShader(modelshader);
	TextureImporter::DestroyTexture(loadedBlockTextures[0]);
	TextureImporter::DestroyTexture(loadedBlockTextures[1]);
	TextureImporter::DestroyTexture(loadedBlockTextures[2]);
	TextureImporter::DestroyTexture(loadedBlockTextures[3]);

	/*for(int i=0;i<SPECULAR_ANIMATION_LENGTH;i++)
		TextureImporter::DestroyTexture(specularMap[0]);*/

	TextureImporter::DestroyTexture(suntexture);

	TextureImporter::DestroyTexture(uiTexture);
	TextureImporter::DestroyTexture(gunTexture);
	TextureImporter::DestroyTexture(inventoryTexture);

	Model::ClearTextures();

	Text::DestroyTTF();

	TextureImporter::DestroyTexture(bulletTexture);
	Sprite::UninitializeSpriteRenderer();

	//glfwSetWindowShouldClose(window,false);
	return;
}

void processInput_NonPhysics_ingame()
{
	//block destruction/placement
	if (ButtonHandler::GetKeyDown(Buttons::BUTTON_MOUSE_LEFT))
		CPS++;
	if (ButtonHandler::GetKey(Buttons::BUTTON_MOUSE_LEFT))
	{
		leftPressed = true;
	}
	else
	{
		leftPressed = false;
		hasBroken = false;
		lastBlockBreak = 0.0f;

		lastGepfegyver = 0.0f;
	}

	if (ButtonHandler::GetKey(Buttons::BUTTON_MOUSE_RIGHT))
	{
		rightPressed = true;
	}
	else
	{
		rightPressed = false;
		hasPlaced = false;
		lastBlockPlacement = 0.0f;
	}

	//blockselect
	bool talalt = false;
	for (int i = 0; i < 9; i++)
	{
		if (ButtonHandler::GetKeyDown((enum Buttons::ButtonNames)(Buttons::BUTTON_1 + i)))
		{
			talalt = true;

			currentBlock = i;
			slotChanged = true;

			break;
		}
	}

	//andere
	if (ButtonHandler::GetKeyDown(Buttons::BUTTON_V))//glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS
	{
		polygonMode = polygonMode == GL_FILL ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	}


	if (ButtonHandler::GetKey(Buttons::BUTTON_C) || (rightPressed && inGepfegyver))
		inZoom = true;
	else inZoom = false;


	if (ButtonHandler::GetKey(Buttons::BUTTON_LEFT_CONTROL) && ButtonHandler::GetKey(Buttons::BUTTON_W))
	{
		inSprint = true;
	}


	if (ButtonHandler::GetKeyDown(Buttons::BUTTON_G))
	{
		if (inGodMode)
		{
			inGodMode = false;
			godModeAborted = true;
		}
		else
		{
			inGodMode = true;
			godModeInitiated = true;
		}
	}

	if (ButtonHandler::GetKeyUp(Buttons::BUTTON_J))//changing block texture style
	{
		unsigned index = 0;
		for (; index < 4; index++)
			if (blockTexture == loadedBlockTextures[index])
			{index++; break;}

		if (index >= sizeof(loadedBlockTextures) / sizeof(loadedBlockTextures[0]))
			index = 0;

		blockTexture = loadedBlockTextures[index];
	}
}

void processInput_MenuChange(GLFWwindow* window)
{
	/*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);*/
	

	//menu
	if (ButtonHandler::GetKeyDown(Buttons::BUTTON_ESCAPE))
	{
		switch (currentmenu)
		{
			case MENU_INGAME:
				currentmenu = MENU_PAUSE;
				//std::cout << "pause\n";
				SetMouseCallback(window, false, false);
				break;

			case MENU_PAUSE:
				currentmenu = MENU_INGAME;
				//std::cout << "ingame\n";
				SetMouseCallback(window, true, true);
				break;

		}
	}

	if (ButtonHandler::GetKeyDown(Buttons::BUTTON_E))
	{
		switch (currentmenu)
		{
		case MENU_INGAME:
			currentmenu = MENU_INVENTORY;
			//std::cout << "pause\n";
			SetMouseCallback(window, false, false);
			break;

		case MENU_INVENTORY:
			currentmenu = MENU_INGAME;
			std::cout << "ingame\n";
			SetMouseCallback(window, true, true);
			break;

		}
	}

}


glm::vec3 processMovementInput_Godmode(Camera& cam);
glm::vec3 processMovementInput_Land(Camera& cam);
glm::vec3 processMovementInput_Water(Camera& cam);

glm::vec3 processMovementInput(GLFWwindow* window, Camera& cam)
{
	/*
	//movement
	glm::vec3 mov = glm::vec3(0.0f);

	glm::vec3 velocity = cam.GetVelocity();
	cam.SetVelocity(velocity, true);
	//std::cout << "velocity: " << velocity.x << " " << velocity.y << " " << velocity.z << std::endl;


	float elore = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_W))
	{
		if (inSprint) elore += MOVEMENT_SPEED * SPRINT_MOVEMENT_SPEED_MULTIPLIER;
		else elore += MOVEMENT_SPEED;
	}
	else
	{
		inSprint = false;
	}

	if (ButtonHandler::GetKey(Buttons::BUTTON_S))
	{
		elore -= MOVEMENT_SPEED;
	}
	//cam.Translate(mov * elore);
	glm::vec3 elore_vec = cam.GetFront();
	elore_vec.y = 0;
	elore_vec = glm::normalize(elore_vec);
	mov += elore * elore_vec;
	mov.y = 0;

	float jobbra = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_D))
	{
		jobbra += MOVEMENT_SPEED;
	}
	if (ButtonHandler::GetKey(Buttons::BUTTON_A))
	{
		jobbra -= MOVEMENT_SPEED;
	}
	//cam.Translate(-jobbra * mov);
	mov -= jobbra * cam.GetRight();
	mov.y = 0;


	float felfele = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_SPACE))
	{
		if (fabs(velocity.y) < 0.001f && !jumpStarted)
		{
			felfele += JUMP_SPEED;
			jumpStarted = true;
		}
		else if(inGodMode)
		{
			felfele += MOVEMENT_SPEED;
		}
	}
	if (ButtonHandler::GetKey(Buttons::BUTTON_LEFT_SHIFT) &&inGodMode)
	{
		felfele -= MOVEMENT_SPEED;
	}
	//cam.Translate(felfele * mov);
	mov.y += felfele;

	if (inGodMode)
	{
		velocity = 3.0f*mov;
	}
	else
	{
		velocity.x = SmoothLerp(velocity.x, mov.x, -20.0f);
		velocity.z = SmoothLerp(velocity.z, mov.z, -20.0f);
		velocity.y += mov.y;
	}*/
	glm::vec3 velocity = glm::vec3(0.0);

	if (inGodMode)
		velocity = processMovementInput_Godmode(cam);
	else if (isPlayerInWater[0])
		velocity = processMovementInput_Water(cam);
	else
		velocity = processMovementInput_Land(cam);

	return velocity;
}

glm::vec3 processMovementInput_Godmode(Camera& cam)
{
	//movement
	glm::vec3 mov = glm::vec3(0.0f);

	glm::vec3 velocity = cam.GetVelocity();
	cam.SetVelocity(velocity, true);

	//elore
	float elore = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_W))
	{
		if (inSprint) elore += MOVEMENT_SPEED * SPRINT_MOVEMENT_SPEED_MULTIPLIER;
		else elore += MOVEMENT_SPEED;
	}
	else
	{
		inSprint = false;
	}

	if (ButtonHandler::GetKey(Buttons::BUTTON_S))
	{
		elore -= MOVEMENT_SPEED;
	}

	glm::vec3 elore_vec = cam.GetFront();
	elore_vec.y = 0;
	elore_vec = glm::normalize(elore_vec);
	mov += elore * elore_vec;


	//oldalra
	float jobbra = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_D))
	{
		jobbra += MOVEMENT_SPEED;
	}
	if (ButtonHandler::GetKey(Buttons::BUTTON_A))
	{
		jobbra -= MOVEMENT_SPEED;
	}
	mov -= jobbra * cam.GetRight();


	//fuggoleges
	float felfele = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_SPACE))
	{
		felfele += MOVEMENT_SPEED;
	}
	if (ButtonHandler::GetKey(Buttons::BUTTON_LEFT_SHIFT))
	{
		felfele -= MOVEMENT_SPEED;
	}
	mov.y += felfele;



	//finalizing
	velocity = 3.0f * mov;
	return velocity;
}

glm::vec3 processMovementInput_Land(Camera& cam)
{
	//movement
	glm::vec3 mov = glm::vec3(0.0f);

	glm::vec3 velocity = cam.GetVelocity();
	cam.SetVelocity(velocity, true);

	//elore
	float elore = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_W))
	{
		if (inSprint) elore += MOVEMENT_SPEED * SPRINT_MOVEMENT_SPEED_MULTIPLIER;
		else elore += MOVEMENT_SPEED;
	}
	else
	{
		inSprint = false;
	}

	if (ButtonHandler::GetKey(Buttons::BUTTON_S))
	{
		elore -= MOVEMENT_SPEED;
	}

	glm::vec3 elore_vec = cam.GetFront();
	elore_vec.y = 0;
	elore_vec = glm::normalize(elore_vec);
	mov += elore * elore_vec;


	//oldalra
	float jobbra = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_D))
	{
		jobbra += MOVEMENT_SPEED;
	}
	if (ButtonHandler::GetKey(Buttons::BUTTON_A))
	{
		jobbra -= MOVEMENT_SPEED;
	}
	mov -= jobbra * cam.GetRight();


	//fuggolegesen
	float felfele = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_SPACE))
	{
		if (fabs(velocity.y) < 0.001f && !jumpStarted)
		{
			felfele += JUMP_SPEED;
			jumpStarted = true;
		}
	}
	mov.y += felfele;

	//finalizing (interpolation and applying gravity)
	velocity.x = SmoothLerp(velocity.x, mov.x, -20.0f);
	velocity.z = SmoothLerp(velocity.z, mov.z, -20.0f);
	velocity.y += mov.y;

	velocity.y -= 20.0 * fixedDeltaTime;
	if (velocity.y < -20.0f)
		velocity.y = -20.0f;


	return velocity;
}

glm::vec3 processMovementInput_Water(Camera& cam)
{
	//movement
	glm::vec3 mov = glm::vec3(0.0f);

	glm::vec3 velocity = cam.GetVelocity();
	cam.SetVelocity(velocity, true);

	//std::cout << isPlayerInWater[0] << " " << isPlayerInWater[1] << '\n';

	//elore
	float elore = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_W))
	{
		if (inSprint) elore += MOVEMENT_SPEED_WATER * SPRINT_MOVEMENT_SPEED_MULTIPLIER;
		else elore += MOVEMENT_SPEED_WATER;
	}
	else
	{
		inSprint = false;
	}

	if (ButtonHandler::GetKey(Buttons::BUTTON_S))
	{
		elore -= MOVEMENT_SPEED_WATER;
	}

	glm::vec3 elore_vec = cam.GetFront();
	elore_vec.y = 0;
	elore_vec = glm::normalize(elore_vec);
	mov += elore * elore_vec;


	//oldalra
	float jobbra = 0.0f;
	if (ButtonHandler::GetKey(Buttons::BUTTON_D))
	{
		jobbra += MOVEMENT_SPEED_WATER;
	}
	if (ButtonHandler::GetKey(Buttons::BUTTON_A))
	{
		jobbra -= MOVEMENT_SPEED_WATER;
	}
	mov -= jobbra * cam.GetRight();


	//fuggolegesen
	float felfele = 0.0f;
	bool jumped = false;
	if (ButtonHandler::GetKey(Buttons::BUTTON_SPACE))
	{
		if (isPlayerInWater[1])
		{
			felfele += 0.5f*JUMP_SPEED * fixedDeltaTime;
		}
		else if (collisionType == Physics::COLLISION_EDGE)
		{
			felfele = JUMP_SPEED;
			jumped = true;
		}
		else
			felfele -= 5.0f * fixedDeltaTime;
	}
	else
		felfele -= 5.0f * fixedDeltaTime;

	mov.y += felfele;

	//finalizing (interpolation and applying gravity)
	velocity.x = SmoothLerp(velocity.x, mov.x, -20.0f);
	velocity.z = SmoothLerp(velocity.z, mov.z, -20.0f);
	velocity.y += mov.y;

	velocity.y = velocity.y < -2.5f ? -2.5f : velocity.y;
	if(jumped)
		velocity.y = velocity.y > JUMP_SPEED ? JUMP_SPEED : velocity.y;
	else
		velocity.y = velocity.y > 2.5f ? 2.5f : velocity.y;


	return velocity;
}


bool processGepfegyverActivationInput(enum activate_gepfegyver& _activationstage, bool& _inGepfegyver)
{
	using namespace Buttons;

	if (ButtonHandler::GetKeyDown(BUTTON_M))
		_activationstage = ACTIVATE_M;
	else if (ButtonHandler::GetKeyDown(BUTTON_O))
	{
		if (_activationstage == ACTIVATE_M)
			_activationstage = ACTIVATE_MO;
		else
			_activationstage = ACTIVATE_;
	}
	else if (ButtonHandler::GetKeyDown(BUTTON_R))
	{
		if (_activationstage == ACTIVATE_MO)
			_activationstage = ACTIVATE_MOR;
		else
			_activationstage = ACTIVATE_;
	}
	else if (ButtonHandler::GetKeyDown(BUTTON_B))
	{
		if (_activationstage == ACTIVATE_MOR)
			_activationstage = ACTIVATE_MORB;
		else
			_activationstage = ACTIVATE_;
	}
	else if (ButtonHandler::GetKeyDown(BUTTON_I))
	{
		if (_activationstage == ACTIVATE_MORB)
			_activationstage = ACTIVATE_MORBI;
		else
			_activationstage = ACTIVATE_;
	}
	else if (ButtonHandler::GetKeyDown(BUTTON_U))
	{
		if (_activationstage == ACTIVATE_MORBI)
			_activationstage = ACTIVATE_MORBIU;
		else
			_activationstage = ACTIVATE_;
	}
	else if (ButtonHandler::GetKeyDown(BUTTON_S))
	{
		if (_activationstage == ACTIVATE_MORBIU)
			_activationstage = ACTIVATE_MORBIUS;
		else
			_activationstage = ACTIVATE_;
	}
	else if (ButtonHandler::GetKeyDown(BUTTON_ENTER))
	{
		if (_activationstage == ACTIVATE_MORBIUS)
		{
			_activationstage = ACTIVATE_;

			_inGepfegyver = _inGepfegyver ? false : true;

			return true;
		}
		else
			_activationstage = ACTIVATE_;
	}

	return false;
}



//callbacks
void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	viewportWidth = width;
	viewportHeight = height;

	lastX = (float)width / 2.0f;
	lastY = (float)height / 2.0f;

	aspectRatio = (float)viewportWidth / (float)viewportHeight;

	firstMouse = true;

	viewportChanged = true;
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0.001)
	{
		currentBlock--;
		slotChanged = true;
		if (currentBlock < 0)
			currentBlock=8;
	}
	else if (yoffset < -0.001)
	{
		currentBlock++;
		slotChanged = true;
		if (currentBlock >=9)
			currentBlock = 0;
	}
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		firstMouse = false;
		return;
	}
	float offsetX = xpos - lastX;
	float offsetY = ypos - lastY;
	/*if (inZoom)
	{
		offsetX *= 0.5f;
		offsetY *= 0.5f;
	}*/

	if (cam_voidp != nullptr)
	{
		float deltaPitch = 0.0f;
		float deltaYaw = 0.0f;
		deltaPitch -= offsetY * MOUSE_SENSITIVITY;
		deltaYaw -= offsetX * MOUSE_SENSITIVITY;

		if (inZoom)
		{
			deltaPitch *= 0.3f;
			deltaYaw *= 0.3f;
		}

		(*((Camera*)cam_voidp)).Rotate(deltaYaw, deltaPitch);
	}

	glfwSetCursorPos(window, lastX, lastY);
}

void mouseCallback_notFixed(GLFWwindow* window, double xpos, double ypos)
{
	//cursorPos[0] = 2.0 * (xpos / viewportWidth) - 1.0;
	//cursorPos[1] = (1.0-(ypos/viewportHeight))*2.0-1.0;
	cursorPos[0] = xpos;
	cursorPos[1] = viewportHeight - ypos;
	//printf("xpos: %.2lf, ypos: %.2lf\n", cursorPos[0], cursorPos[1]);
}

float SmoothLerp(float a, float b, float speed)
{
	return  a + (b - a) * (1.0f - powf(2.71828f, speed * fixedDeltaTime));
	   // Lerp(fov_current, goal, 1.0f - powf(2.71828f, -10.0f * deltaTime));
}


//canvas

void RefreshHotbar(Canvas& canvas)
{
	char name_temp[40];
	strcpy(name_temp, "slot0");
	for (unsigned int i = 0; i < 9; i++)
	{
		name_temp[4] = name_temp[4] + 1;


		float _pos[2];
		BlockDatabase::GetUv(0, 0, hotbar[i], _pos[0], _pos[1]);
		canvas[name_temp]->SetUVPosition(_pos[0], _pos[1]);

		if (hotbar[i] == BORSOD)
			canvas[name_temp]->SetUVScale(0.4f, 0.4f);
		else
			canvas[name_temp]->SetUVScale(0.1f, 0.1f);
	}
}

//restarto

void defaultGlobalVariables_Game()
{
	lastX = 600.0f;
	lastY = 300.0f;

	leftPressed = false; hasBroken = false; lastBlockBreak = 0.0f;
	rightPressed = false; hasPlaced = false; lastBlockPlacement = 0.0f;

	lastCPS = 0;

	jumpStarted = false;
	inSprint = false;
	inGodMode = false; godModeInitiated = false; godModeAborted = false;

	collisionType = Physics::COLLISION_NONE;
	currentBlock = 0;
	for (size_t i = 0; i < 9; i++) hotbar[i] = AIR;
	slotChanged = false;
	hotbarContentChanged = false;

	lastTextRefresh = 0.0;

	inGepfegyver = false;
	inGepfegyverChanged = false;
	gepfegyverActivationState = ACTIVATE_;

	currentFOV = NORMAL_FOV;
	inZoom = false;

	currentmenu = MENU_INGAME;
}

void defaultGlobalVariables_Menu()
{
	hasStarted = false;
	hasQuit = false;
}