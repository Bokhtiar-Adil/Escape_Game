/*
	Developed by -
	Bokhtiar Adil Prottoy, 
	Department of Computer Science and Engineering,
	Khulna University of Engineering & Technology, 
	Khulna, Bangladesh
*/

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Shader.h"
#include "Headers/Camera.h"
//#include "Text.h"
#include "Headers/Components.h"
#include "Headers/World.h"
#include "Headers/Character.h"
#include "Headers/Curves.h"
#include "Headers/CollectorItems.h"

using namespace std;

float initialCameraZ = 8.0f;
Camera camera(glm::vec3(1.0f, 0.8f, initialCameraZ));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

int WIN_WIDTH = 1200;
int WIN_HEIGHT = 700;

unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 700;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void processInputForBeforeGameStarts(GLFWwindow* window);
void processInputForGameFinishingMode(GLFWwindow* window);
void processInputForGhostMode(GLFWwindow* window);

unsigned int dummyVAO = NULL;
Shader dummyShader;

// components
void road(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void table(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void box(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void simpleRoom(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f), unsigned int& lightCubeVAO = dummyVAO, Shader& lightCubeShader = dummyShader);


// helper functions
void shaderSetup(Shader& lightCubeShader, Shader& shaderTex, Shader& shaderMP, Shader& shaderSky, Shader& shaderCurves, glm::mat4& projection, glm::mat4& view);
void worldExpansion(Shader& shaderTex, Shader& shaderMP, Shader& shaderCurves, Shader& lightCubeShader, World& world, Components& component, vector<int>& sequence, glm::mat4 alTogether);
void protagonistMoveManager(Character& protagonist, Shader& shaderMP, Shader& lightCubeShader, glm::mat4 revolve);
void streetlightSetup(Shader& shader, float moveZ, float slAmb = 0.1f, float slDiff = 0.5f, float slSpec = 0.5f, float slConst = 1.0f, float slLin = 0.09f, float slQuad = 0.032f);
void dayNightControl();
void collectorItemsManager(Shader& shader1, Shader& shader2, CollectorItems& items, World& world, Components& component);
void bonusManager();
void gameFreezeManager(Shader& shaderTex, Shader& shaderMP, Components& component, Character& antagonist);
void writeWelcomMsgCmd();


bool torchOn = false;
bool nightMode = false;
bool streetLightOn = false;
int numOfPointLightRoad = 3;
int numOfPointLightRoom = 1;
bool ambientOn = true;
bool diffuseOn = true;
bool specularOn = true;

float rotate_obj_x = 0.0f;
float rotate_obj_y = 0.0f;
float rotate_obj_z = 0.0f;
float rotate_obj_axis_x = 0.0f;
float rotate_obj_axis_y = 0.0f;
float rotate_obj_axis_z = 0.0f;

float currentBlockBase = 0.0f;
float currentCharacterPos = initialCameraZ;
int currentBlockNumber = 0;
float protagonistXmove = 0.0f, protagonistYmove = 0.0f, protagonistZmove = 0.0f;
float protagonistXinitial = 1.05f, protagonistYinitial = 0.4f, protagonistZinitial = 5.0f;
float protagonistXcurrent = protagonistXinitial + protagonistXmove, protagonistYcurrent = protagonistYinitial + protagonistYmove, protagonistZcurrent = protagonistZinitial + protagonistZmove;
int jumpCoolDown = 0;
bool jumping = false;
int protagonistMovementForm = 0, protagonistMovementFormCounter = 0;


float sunX = 1.05f;
float sunY = 3.0f;
float sunZ = 5.0f;
int nightDuration = 0;
float sunAmb = 0.3f, sunDiff = 0.8f, sunSpec = 1.0f;
bool dayNightCycleMode = true;
bool dayNightDirectMode = false;
float dayNightSystem = true;
float moonX = 6.0f;
float moonY = 0.0f;
float moonZ = sunZ;

int numOfBlockComponent = 7, numOfStreetLight = 7, numOfResidential = 2, typeOfBlockComponent = 2;

float itemAngleChange = 1.0f;

bool menu = true;

GLint viewport[4];

glm::mat4 projection = glm::mat4(1.0f), view = glm::mat4(1.0f);
glm::mat4 revolve = glm::mat4(1.0f);

vector<int> bonusItemSequence, bonusItemXPositions, bonusItemYPositions, bonusItemZPositions;
float bonusItemOffset = 20.0f;
int boostBonusEffectDuration = 0, darkBonusEffectDuration = 0, slowBonusEffectDuration = 0, maxBoostBonusEffectDuration = 1000;
int maxDarkBonusEffectDuration = 500, maxSlowBonusEffectDuration;
bool boostBonusAchieved = false, darkBonusAchieved = false, slowBonusAchieved = false;
int fuel = 15, coins = 0, boostFactor = 1;

float gameStarted = false, gameFinished = false, gameWon = false, gameLost = false;
float gameStartTime = 0.0f;

int protagonistDelay = 0;
bool bigRobotCameo = false;

float wideView = false;
bool ghostMode = false;
float ghostTImer = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Escape Game", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to load window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// glad loading

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to load GLAD\n";
		return -1;
	}

	// shader creation

	Shader shaderMP("Shaders/vsSrc.vs", "Shaders/fsSrcMatProp.fs");
	Shader shaderTex("Shaders/vsSrc.vs", "Shaders/fsSrcTex.fs");
	Shader shaderSky("Shaders/vsSrc.vs", "Shaders/fsSrcTex.fs");
	Shader shader("Shaders/vsSrc.vs", "Shaders/fsSrcBoth.fs");
	Shader lightCubeShader("Shaders/lightCube.vs", "Shaders/lightCube.fs");
	Shader textShader("Shaders/text_2d.vs", "Shaders/text_2d.fs");
	Shader shaderCurves("Shaders/vsSrcCurves.vs", "Shaders/fsSrcCurves.fs");


	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 offset, altogether, translate, rotate, scale, identity = glm::mat4(1.0f);
	float xoffset = 0.0f, yoffset = 0.0f, zoffset = 0.0f;

	Components component(WIN_WIDTH, WIN_HEIGHT);
	World world;
	Character protagonist;
	// Text text(textShader, SCR_WIDTH, SCR_HEIGHT);
	Curves hatCurves, treeCurves;
	CollectorItems items;

	float px = 1.0f, py = 0.2f, pz = initialCameraZ;
	vector<int> sequence;

	for (int i = 0; i < numOfBlockComponent; i++) {
		sequence.push_back(rand() % typeOfBlockComponent);
	}

	glGetIntegerv(GL_VIEWPORT, viewport);

	/*text.Load("Antonio-Bold.ttf", 48);
	text.RenderText("This is a sample text", 0.0f, 0.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	glfwMakeContextCurrent(window);*/

	hatCurves.setViewport(viewport);
	treeCurves.setViewport(viewport);

	vector<float> dummycntrlpoints = { 223, 65, 232, 90, 232, 145, 219, 191, 210, 245, 232, 278, 268, 300, 309, 307, 351, 307, 384, 304 };
	vector<float> treeTopPoints = { 632, 48, 612, 38, 595, 24, 575, 18, 552, 20, 531, 29, 507, 51, 498, 79, 499, 104, 514, 123, 524, 135, 506, 147, 486, 155, 461, 160, 433, 174, 419, 192, 404, 221, 399, 247, 419, 266, 442, 280, 474, 298, 485, 313, 465, 319, 443, 334, 418, 354, 395, 375, 364, 400, 351, 434, 347, 478, 364, 516, 394, 541, 425, 557, 481, 575, 521, 587, 566, 572, 592, 548, 620, 509, 631, 493, 647, 460 };
	vector<float> tree2 = { 540, 39, 483, 100, 452, 164, 424, 247, 402, 316, 393, 393, 421, 447, 473, 485, 529, 492, 583, 480, 615, 431 };
	vector<float> hat = { 642, 78, 569, 58, 495, 55, 459, 107, 416, 148, 340, 164, 289, 238, 192, 247, 119, 354, 45, 434 };
	vector<float> tree3 = { 611, 48, 572, 46, 507, 56, 435, 89, 407, 133, 380, 207, 394, 260, 375, 297, 319, 357, 299, 440, 364, 511, 437, 554, 502, 568, 549, 559, 579, 519 };
	
	hatCurves.setControlPoints(hat);
	treeCurves.setControlPointsV2(tree3);
	hatCurves.setWinProperties(1200, 700);
	treeCurves.setWinProperties(1200, 700);

	int temp;
	for (int i = 0; i < 15; i++) {
		temp = rand() % 5;
		bonusItemSequence.push_back(temp);
		bonusItemZPositions.push_back(protagonistZinitial - 2.0f - i * 1.0f);
		temp = rand() % 3;
		bonusItemXPositions.push_back(0.0f + temp);
		bonusItemYPositions.push_back(0.4f);
	}

	float prevTime = 0.0f;

	writeWelcomMsgCmd();

	bool ten = false, twe = false, thi = false, fo = false, fi = false, si = false;

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (ghostMode) ghostTImer += deltaTime;

		if (gameStarted && !ghostMode) protagonistDelay++;

		if ((floor(currentFrame - gameStartTime) == 10.0f && !ten) || (floor(currentFrame - gameStartTime) == 20.0f && !twe) || (floor(currentFrame - gameStartTime) == 30.0f && !thi) || (floor(currentFrame - gameStartTime) == 40.0f && !fo) || (floor(currentFrame - gameStartTime) == 50.0f && !fi) || (floor(currentFrame - gameStartTime) == 60.0f && !si)) {
			cout << "Time played: " << floor(currentFrame - gameStartTime) << "s -- Fuel: " << fuel << " -- Coins: " << coins << "\n";
			if (!ten) ten = true;
			else if (!twe) twe = true;
			else if (!thi) thi = true;
			else if (!fo) fo = true;
			else if (!fi) fi = true;
			else if (!si) si = true;

		}

		if (gameStarted && !ghostMode && !gameLost && currentFrame - gameStartTime >= 60.0f) {
			gameWon = true;
			gameLost = false;
			gameFinished = true;
		}
		else if (gameStarted && !ghostMode && !gameWon && (fuel == 0 || protagonistDelay >= 100)) {
			gameLost = true;
			gameWon = false;
			gameFinished = true;
			if (protagonistDelay >= 50) bigRobotCameo = true;
		}

		if (!gameStarted) {
			dayNightSystem = false;
			processInputForBeforeGameStarts(window);
		}
		else if (gameFinished) processInputForGameFinishingMode(window);
		else if (ghostMode) processInputForGhostMode(window);
		else processInput(window);

		if (gameStarted && !ghostMode) prevTime += deltaTime;
		if (prevTime >= 1.0f) {
			if (fuel > 0) fuel--;
			if (coins > 0) coins -= 2;
			prevTime = 0.0f;
		}

		if (fuel < 5 && coins > 0) {
			fuel += 2;
			coins--;
		}

		jumpCoolDown++;
		if (jumpCoolDown > 20)
			jumping = false;


		if (dayNightSystem) {
			if (!nightMode) glClearColor(sunDiff, sunDiff, sunDiff, 1.0f);
			else glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else {
			if (!darkBonusAchieved) glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 30.0f);
		view = camera.GetViewMatrix();

		shaderSetup(lightCubeShader, shaderTex, shaderMP, shaderSky, shaderCurves, projection, view);

		if (!gameStarted) {
			gameFreezeManager(shaderTex, shaderMP, component, protagonist);
		}
		else if (gameFinished) {
			gameFreezeManager(shaderTex, shaderMP, component, protagonist);
		}
		else {

		}

		xoffset = 0.5f, yoffset = 0.5f;
		offset = glm::translate(identity, glm::vec3(xoffset, yoffset, zoffset));

		glm::mat4 rotateXMatrix = glm::rotate(identity, glm::radians(rotate_obj_x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotateYMatrix = glm::rotate(identity, glm::radians(rotate_obj_y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotateZMatrix = glm::rotate(identity, glm::radians(rotate_obj_z), glm::vec3(0.0f, 0.0f, 1.0f));
		revolve = rotateZMatrix * rotateYMatrix * rotateXMatrix;


		scale = glm::scale(identity, glm::vec3(0.2f, 0.2f, 0.2f));
		translate = glm::translate(identity, glm::vec3(protagonistXinitial, 0.4f, protagonistZinitial));
		hatCurves.setModel(glm::translate(identity, glm::vec3(protagonistXmove, protagonistYmove, protagonistZmove)) * translate * scale * revolve);
		treeCurves.setModel(glm::translate(identity, glm::vec3(0.0f, 3.0f, 1.0f)) * translate * revolve);

		shaderCurves.setBool("overrideColor", false);
		hatCurves.drawCurves(shaderCurves);

		currentBlockBase = 0.0f;
		if ((-1 * camera.Position.z + initialCameraZ) - currentBlockNumber * 30.0f > 30.0f) {
			currentBlockNumber += 1;
		}
		currentBlockBase = -1 * (currentBlockNumber * 30.0f);
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, currentBlockBase + 0.0f));
		worldExpansion(shaderTex, shaderMP, shaderCurves, lightCubeShader, world, component, sequence, translate);
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, currentBlockBase - 30.0f));
		worldExpansion(shaderTex, shaderMP, shaderCurves, lightCubeShader, world, component, sequence, translate);

		protagonistMoveManager(protagonist, shaderMP, lightCubeShader, revolve);
		protagonistXcurrent = protagonistXinitial + protagonistXmove;
		protagonistYcurrent = protagonistYinitial + protagonistYmove;
		protagonistZcurrent = protagonistZinitial + protagonistZmove;
		collectorItemsManager(shaderMP, lightCubeShader, items, world, component);

		if (dayNightSystem) {
			if (!nightMode) component.sun(lightCubeShader, glm::translate(identity, glm::vec3(sunX, sunY + 2.0f, protagonistZmove - 20.0f)));
			if (streetLightOn) component.moon(lightCubeShader, glm::translate(identity, glm::vec3(moonX, moonY + 2.0f, protagonistZmove - 20.0f)));
		}
		else {
			if (!nightMode) component.sun(lightCubeShader, glm::translate(identity, glm::vec3(sunX, sunY + 2.0f, protagonistZmove - 20.0f)));
			if (streetLightOn) component.moon(lightCubeShader, glm::translate(identity, glm::vec3(moonX, moonY + 2.0f, protagonistZmove - 20.0f)));
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	shader.deleteProgram();

	glfwTerminate();
	return 0;
}


/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInputForGhostMode(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		menu = !menu;

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (camera.permissionToMove()) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
			
		}

	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (camera.permissionToMove()) {
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}

	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		if (camera.permissionToMove()) {
			camera.ProcessKeyboard(LEFT, deltaTime);
		}

	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (camera.permissionToMove()) {
			camera.ProcessKeyboard(RIGHT, deltaTime);
		}

	}
	// camera movement control
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		camera.RotateAroundAxis(1, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		camera.RotateAroundAxis(2, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		camera.RotateInverseAroundAxis(2, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		camera.RotateAroundAxis(3, 3.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		camera.Orbit();
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		camera.ResetPosition();
		ghostMode = false;
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		menu = !menu;

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (camera.permissionToMove()) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
			currentCharacterPos += 0.05f;
			protagonistZmove -= (camera.MovementSpeed * deltaTime);
			if (protagonistMovementFormCounter == 0) {
				protagonistMovementForm = 1;
			}
			protagonistDelay = 0;
		}

	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// no movement backwards in during game time -- only allowed in ghost mode
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		if (camera.permissionToMove()) {			
			if (protagonistXmove - (camera.MovementSpeed * deltaTime) > -1.7f) {
				camera.ProcessKeyboard(LEFT, deltaTime);
				protagonistXmove -= (camera.MovementSpeed * deltaTime);
			}
			if (protagonistMovementFormCounter == 0) {
				protagonistMovementForm = 1;
			}
		}

	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (camera.permissionToMove()) {			
			if (protagonistXmove + (camera.MovementSpeed * deltaTime) < 1.6f) {
				camera.ProcessKeyboard(RIGHT, deltaTime);
				protagonistXmove += (camera.MovementSpeed * deltaTime);
			}
			if (protagonistMovementFormCounter == 0) {
				protagonistMovementForm = 1;
			}
		}

	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (camera.permissionToMove()) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
			currentCharacterPos += 0.05f;
			protagonistZmove -= (camera.MovementSpeed * deltaTime);
			if (jumpCoolDown >= 20) {
				protagonistYmove = (25.0f * camera.MovementSpeed * deltaTime);
				jumpCoolDown = 0;
				jumping = true;
			}
		}

	}
	// camera movement control
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		camera.RotateAroundAxis(1, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		camera.RotateAroundAxis(2, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		camera.RotateInverseAroundAxis(2, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		camera.RotateAroundAxis(3, 3.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		camera.Orbit();
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		glm::vec4 identity = glm::vec4(1.0f);
		glm::vec4 protagonistInitial = glm::vec4(protagonistXinitial, protagonistYinitial, protagonistZinitial, 1.0f);
		glm::vec4 protagonistMove = glm::vec4(protagonistXmove, protagonistYmove, protagonistZmove, 1.0f);
		glm::vec4 pos = protagonistMove * protagonistInitial;
		camera.ResetPosition();
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		camera.wideView();
	}
	/**/
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		torchOn = !torchOn;
	}
	// nightmode contol
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		if (nightMode == false) {
			dayNightCycleMode = false;
			nightMode = true;
			dayNightDirectMode = true;
			dayNightControl();
		}
		else {
			nightMode = false;
			dayNightDirectMode = false;
			dayNightControl();
		}

	}

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		if (ghostMode == false) {
			ghostMode = true;
			camera.saveVectors();
			
		}

	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		numOfPointLightRoom += 1;
		numOfPointLightRoom %= 2;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		numOfPointLightRoad += 3;
		if (numOfPointLightRoad > 3) numOfPointLightRoad = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		rotate_obj_x += 0.1;
		rotate_obj_axis_x = 1.0;
		rotate_obj_axis_y = 0.0;
		rotate_obj_axis_z = 0.0;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		rotate_obj_y += 0.5;
		rotate_obj_axis_x = 0.0;
		rotate_obj_axis_y = 1.0;
		rotate_obj_axis_z = 0.0;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		rotate_obj_z += 0.1;
		rotate_obj_axis_x = 0.0;
		rotate_obj_axis_y = 0.0;
		rotate_obj_axis_z = 1.0;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		rotate_obj_x = 0.0;
		rotate_obj_y = 0.0;
		rotate_obj_z = 0.0;
		rotate_obj_axis_x = 0.0;
		rotate_obj_axis_y = 0.0;
		rotate_obj_axis_z = 1.0;
	}
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		sunX = 3.0f;
		sunY = 3.0f;
		moonX = 3.0f;
		moonY = 3.0f;
		dayNightSystem = !dayNightSystem;
	}


}

void processInputForGameFinishingMode(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		if (gameWon) {
			cout << "\n\nCongratulations! You escaped from the big robot! You won!\n\n";
		}
		else if (gameLost) {
			cout << "\n\nDon't worry! Better luck next time!\n\n";
		}
	}
		
}

void processInputForBeforeGameStarts(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		gameStarted = true;
		dayNightSystem = true;
		gameStartTime = static_cast<float>(glfwGetTime());
	}

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void dayNightControl()
{
	if (!dayNightSystem) return;
	//sunZ = protagonistZmove + 5.0f;
	if (dayNightCycleMode) {
		float gradualSunLightChange = 0.001f;
		float gradualSunPositionChange = 0.01f;
		if (sunX > -6.0f) {
			if (sunAmb < 0.3f) sunAmb += gradualSunLightChange;
			if (sunDiff < 0.8f) sunDiff += gradualSunLightChange;
			if (sunSpec < 1.0f) sunSpec += gradualSunLightChange;
			if (sunAmb >= 0.3f && sunDiff >= 0.8f && sunSpec >= 1.0f) sunX -= gradualSunPositionChange;
			if (sunY < 3.0f) sunY += gradualSunPositionChange;
		}
		else {
			sunY -= gradualSunPositionChange;
			if (sunAmb > 0.0f) sunAmb -= gradualSunLightChange;
			if (sunDiff > 0.0f) sunDiff -= gradualSunLightChange;
			if (sunSpec > 0.0f) sunSpec -= gradualSunLightChange;
			if (sunDiff <= 0.0f) nightMode = true;
			if (nightMode) {
				nightDuration++;
				moonX -= gradualSunPositionChange;
			}
			if (nightDuration == 1200) {
				nightMode = false;
				nightDuration = 0;
				sunX = 6.0f;
				sunY = 0.0f;
				moonX = sunX;
				moonY = 0.0f;
			}
		}
		if (sunDiff <= 0.3f) {
			streetLightOn = true;
			if (moonY < 3.0f) moonY += gradualSunPositionChange;
		}
		else streetLightOn = false;
	}
	else {
		if (!dayNightDirectMode) {
			sunX = 1.05f;
			sunY = 3.0f;
			sunZ = 5.0f;
			nightDuration = 0;
			sunAmb = 0.3f;
			sunDiff = 0.8f;
			sunSpec = 1.0f;
			nightMode = false;
			dayNightCycleMode = true;
			streetLightOn = false;
		}
		else {
			if (nightMode) {
				sunAmb = 0.0f;
				sunDiff = 0.0f;
				sunSpec = 1.0f;
				streetLightOn = true;
			}
			else {
				sunAmb = 0.3f;
				sunDiff = 0.8f;
				sunSpec = 1.0f;
				dayNightCycleMode = true;
				streetLightOn = false;
			}
		}

	}
}

void shaderSetup(Shader& lightCubeShader, Shader& shaderTex, Shader& shaderMP, Shader& shaderSky, Shader& shaderCurves, glm::mat4& projection, glm::mat4& view)
{
	lightCubeShader.use();
	lightCubeShader.setMat4("projection", projection);
	lightCubeShader.setMat4("view", view);

	// light properties
	dayNightControl();

	// shaderTex

	shaderTex.use();
	shaderTex.setMat4("projection", projection);
	shaderTex.setMat4("view", view);
	if (nightMode) shaderTex.setBool("nightMode", true);
	else shaderTex.setBool("nightMode", false);
	shaderTex.setBool("flashlightOn", false);
	shaderTex.setInt("numberofPointlights", 0);
	shaderTex.setVec3("viewPos", camera.Position);

	shaderTex.setVec3("dirLight.direction", -sunX, -sunY, -sunZ);
	shaderTex.setVec3("dirLight.ambient", sunAmb, sunAmb, sunAmb);
	shaderTex.setVec3("dirLight.diffuse", sunDiff, sunDiff, sunDiff);
	shaderTex.setVec3("dirLight.specular", sunSpec, sunSpec, sunSpec);
	shaderTex.setBool("nightMode", nightMode);

	// spotlight
	shaderTex.setVec3("spotLight.position", camera.Position);
	shaderTex.setVec3("spotLight.direction", camera.Front);
	shaderTex.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shaderTex.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shaderTex.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shaderTex.setFloat("spotLight.constant", 1.0f);
	shaderTex.setFloat("spotLight.linear", 0.09f);
	shaderTex.setFloat("spotLight.quadratic", 0.032f);
	shaderTex.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shaderTex.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	shaderTex.setBool("flashlightOn", torchOn);

	streetlightSetup(shaderTex, protagonistZmove);

	// shaderMP

	shaderMP.use();
	shaderMP.setMat4("projection", projection);
	shaderMP.setMat4("view", view);
	if (nightMode) shaderMP.setBool("nightMode", true);
	else shaderMP.setBool("nightMode", false);
	shaderMP.setBool("flashlightOn", false);
	shaderMP.setInt("numberofPointlights", 0);
	shaderMP.setVec3("viewPos", camera.Position);

	// light properties

	// directional light 
	shaderMP.setVec3("dirLight.direction", -sunX, -sunY, -sunZ);
	shaderMP.setVec3("dirLight.ambient", sunAmb, sunAmb, sunAmb);
	shaderMP.setVec3("dirLight.diffuse", sunDiff, sunDiff, sunDiff);
	shaderMP.setVec3("dirLight.specular", sunSpec, sunSpec, sunSpec);
	shaderMP.setBool("nightMode", nightMode);

	// spotlight
	shaderMP.setVec3("spotLight.position", camera.Position);
	shaderMP.setVec3("spotLight.direction", camera.Front);
	shaderMP.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shaderMP.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shaderMP.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shaderMP.setFloat("spotLight.constant", 1.0f);
	shaderMP.setFloat("spotLight.linear", 0.09f);
	shaderMP.setFloat("spotLight.quadratic", 0.032f);
	shaderMP.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shaderMP.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	shaderMP.setBool("flashlightOn", torchOn);

	streetlightSetup(shaderMP, protagonistZmove);

	// shaderSky

	shaderSky.use();
	shaderSky.setMat4("projection", projection);
	shaderSky.setMat4("view", view);
	if (nightMode) shaderSky.setBool("nightMode", true);
	else shaderSky.setBool("nightMode", false);
	shaderSky.setBool("flashlightOn", false);
	shaderSky.setInt("numberofPointlights", 0);
	shaderSky.setVec3("viewPos", camera.Position);

	// light properties

	shaderSky.setVec3("dirLight.direction", -sunX, sunY, -sunZ);
	shaderSky.setVec3("dirLight.ambient", sunDiff, sunDiff, sunDiff);
	shaderSky.setVec3("dirLight.diffuse", sunDiff, sunDiff, sunDiff);
	shaderSky.setVec3("dirLight.specular", sunDiff, sunDiff, sunDiff);
	shaderSky.setBool("nightMode", nightMode);

	// spotlight
	shaderSky.setVec3("spotLight.position", camera.Position);
	shaderSky.setVec3("spotLight.direction", camera.Front);
	shaderSky.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shaderSky.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shaderSky.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shaderSky.setFloat("spotLight.constant", 1.0f);
	shaderSky.setFloat("spotLight.linear", 0.09f);
	shaderSky.setFloat("spotLight.quadratic", 0.032f);
	shaderSky.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shaderSky.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	shaderSky.setBool("flashlightOn", torchOn);

	// shaderCurves

	shaderCurves.use();
	shaderCurves.setMat4("projection", projection);
	shaderCurves.setMat4("view", view);
	if (nightMode) shaderCurves.setBool("nightMode", true);
	else shaderCurves.setBool("nightMode", false);
	shaderCurves.setBool("flashlightOn", false);
	shaderCurves.setInt("numberofPointlights", 0);
	shaderCurves.setVec3("viewPos", camera.Position);
	shaderCurves.setBool("curve", true);

	// light properties

	// directional light 
	shaderCurves.setVec3("dirLight.direction", -sunX, -sunY, -sunZ);
	shaderCurves.setVec3("dirLight.ambient", sunAmb * 0.5f, sunAmb * 0.5f, sunAmb * 0.5f);
	shaderCurves.setVec3("dirLight.diffuse", sunDiff * 0.5f, sunDiff * 0.5f, sunDiff * 0.5f);
	shaderCurves.setVec3("dirLight.specular", sunSpec, sunSpec, sunSpec);
	shaderCurves.setBool("nightMode", nightMode);

	shaderCurves.setVec3("dirLight2.direction", -sunX, -sunY, sunZ);
	shaderCurves.setVec3("dirLight2.ambient", sunAmb * 0.5f, sunAmb * 0.5f, sunAmb * 0.5f);
	shaderCurves.setVec3("dirLight2.diffuse", sunDiff * 0.5f, sunDiff * 0.5f, sunDiff * 0.5f);
	shaderCurves.setVec3("dirLight2.specular", sunSpec * 0.5f, sunSpec * 0.5f, sunSpec * 0.5f);
	shaderCurves.setBool("nightMode", nightMode);

	// spotlight
	shaderCurves.setVec3("spotLight.position", camera.Position);
	shaderCurves.setVec3("spotLight.direction", camera.Front);
	shaderCurves.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shaderCurves.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shaderCurves.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shaderCurves.setFloat("spotLight.constant", 1.0f);
	shaderCurves.setFloat("spotLight.linear", 0.09f);
	shaderCurves.setFloat("spotLight.quadratic", 0.032f);
	shaderCurves.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shaderCurves.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	shaderCurves.setBool("flashlightOn", torchOn);

	streetlightSetup(shaderCurves, protagonistZmove);

	if (darkBonusAchieved) {
		shaderMP.use();
		shaderMP.setBool("darkBonusAchieved", true);
		shaderTex.use();
		shaderTex.setBool("darkBonusAchieved", true);
		shaderSky.use();
		shaderSky.setBool("darkBonusAchieved", true);
		lightCubeShader.use();
		lightCubeShader.setBool("darkBonusAchieved", true);
		shaderCurves.use();
		shaderCurves.setBool("darkBonusAchieved", true);
		dayNightSystem = false;
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		shaderMP.use();
		shaderMP.setBool("darkBonusAchieved", false);
		shaderTex.use();
		shaderTex.setBool("darkBonusAchieved", false);
		shaderSky.use();
		shaderSky.setBool("darkBonusAchieved", false);
		lightCubeShader.use();
		lightCubeShader.setBool("darkBonusAchieved", false);
		shaderCurves.use();
		shaderCurves.setBool("darkBonusAchieved", false);
		dayNightSystem = true;
		glClearColor(sunDiff, sunDiff, sunDiff, 1.0f);
	}

}

void streetlightSetup(Shader& shader, float moveZ, float slAmb, float slDiff, float slSpec, float slConst, float slLin, float slQuad)
{
	float firstStreetlightXLeft = -0.8f, firstStreetlightXRight = 2.8f, firstStreetlightY = 1.5f, firstStreetlightZ = 8.0f + moveZ;
	glm::vec4 firstStreetlightLeft = glm::vec4(firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ, 1.0f);
	glm::vec4 firstStreetlightRight = glm::vec4(firstStreetlightXRight, firstStreetlightY, firstStreetlightZ, 1.0f);
	glm::vec4 streetlightTogether;
	int numOfStreetLight = 7;
	int gap = -4.0f, index = 0;

	shader.use();
	shader.setInt("numberofStreetlights", 14);
	shader.setBool("streetLightOn", streetLightOn);
	shader.setVec3("streetLights[0].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[0].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[0].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[0].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[0].constant", slConst);
	shader.setFloat("streetLights[0].linear", slLin);
	shader.setFloat("streetLights[0].quadratic", slQuad);
	shader.setBool("streetLightStatus[0]", true);
	index++;

	shader.setVec3("streetLights[1].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[1].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[1].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[1].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[1].constant", slConst);
	shader.setFloat("streetLights[1].linear", slLin);
	shader.setFloat("streetLights[1].quadratic", slQuad);
	shader.setBool("streetLightStatus[1]", true);
	index++;

	shader.setVec3("streetLights[2].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[2].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[2].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[2].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[2].constant", slConst);
	shader.setFloat("streetLights[2].linear", slLin);
	shader.setFloat("streetLights[2].quadratic", slQuad);
	shader.setBool("streetLightStatus[2]", true);
	index++;

	shader.setVec3("streetLights[3].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[3].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[3].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[3].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[3].constant", slConst);
	shader.setFloat("streetLights[3].linear", slLin);
	shader.setFloat("streetLights[3].quadratic", slQuad);
	shader.setBool("streetLightStatus[3]", true);
	index++;

	shader.setVec3("streetLights[4].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[4].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[4].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[4].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[4].constant", slConst);
	shader.setFloat("streetLights[4].linear", slLin);
	shader.setFloat("streetLights[4].quadratic", slQuad);
	shader.setBool("streetLightStatus[4]", true);
	index++;

	shader.setVec3("streetLights[5].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[5].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[5].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[5].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[5].constant", slConst);
	shader.setFloat("streetLights[5].linear", slLin);
	shader.setFloat("streetLights[5].quadratic", slQuad);
	shader.setBool("streetLightStatus[5]", true);
	index++;

	shader.setVec3("streetLights[6].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[6].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[6].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[6].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[6].constant", slConst);
	shader.setFloat("streetLights[6].linear", slLin);
	shader.setFloat("streetLights[6].quadratic", slQuad);
	shader.setBool("streetLightStatus[6]", true);

	index = 0;

	shader.setVec3("streetLights[7].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[7].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[7].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[7].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[7].constant", slConst);
	shader.setFloat("streetLights[7].linear", slLin);
	shader.setFloat("streetLights[7].quadratic", slQuad);
	shader.setBool("streetLightStatus[7]", true);
	index++;

	shader.setVec3("streetLights[8].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[8].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[8].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[8].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[8].constant", slConst);
	shader.setFloat("streetLights[8].linear", slLin);
	shader.setFloat("streetLights[8].quadratic", slQuad);
	shader.setBool("streetLightStatus[8]", true);
	index++;

	shader.setVec3("streetLights[9].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[9].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[9].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[9].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[9].constant", slConst);
	shader.setFloat("streetLights[9].linear", slLin);
	shader.setFloat("streetLights[9].quadratic", slQuad);
	shader.setBool("streetLightStatus[9]", true);
	index++;

	shader.setVec3("streetLights[10].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[10].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[10].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[10].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[10].constant", slConst);
	shader.setFloat("streetLights[10].linear", slLin);
	shader.setFloat("streetLights[10].quadratic", slQuad);
	shader.setBool("streetLightStatus[10]", true);
	index++;

	shader.setVec3("streetLights[11].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[11].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[11].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[11].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[11].constant", slConst);
	shader.setFloat("streetLights[11].linear", slLin);
	shader.setFloat("streetLights[11].quadratic", slQuad);
	shader.setBool("streetLightStatus[11]", true);
	index++;

	shader.setVec3("streetLights[12].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[12].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[12].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[12].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[12].constant", slConst);
	shader.setFloat("streetLights[12].linear", slLin);
	shader.setFloat("streetLights[12].quadratic", slQuad);
	shader.setBool("streetLightStatus[12]", true);
	index++;

	shader.setVec3("streetLights[13].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index * gap);
	shader.setVec3("streetLights[13].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[13].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[13].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[13].constant", slConst);
	shader.setFloat("streetLights[13].linear", slLin);
	shader.setFloat("streetLights[13].quadratic", slQuad);
	shader.setBool("streetLightStatus[13]", true);
	index++;

}

void worldExpansion(Shader& shaderTex, Shader& shaderMP, Shader& shaderCurves, Shader& lightCubeShader, World& world, Components& component, vector<int>& sequence, glm::mat4 alTogether)
{
	glm::mat4 rotate, scale, translate, identity = glm::mat4(1.0f), rotateMosque;

	shaderTex.use();

	scale = glm::scale(identity, glm::vec3(0.8f, 0.5f, 0.5f));
	rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	component.billboard_noexit(shaderTex, shaderMP, alTogether * glm::translate(identity, glm::vec3(3.0f, -0.5f, 7.0f)) * rotate * scale);
	rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	component.billboard_noexit(shaderTex, shaderMP, alTogether * glm::translate(identity, glm::vec3(-1.0f, -0.5f, 7.0f)) * rotate * scale);

	for (int i = 0; i < numOfResidential; i++) {
		world.residential(shaderTex, true, true, alTogether * glm::translate(identity, glm::vec3(-4.0f, -0.5f, -6.0f - i * 15.0f)));
		world.residential(shaderTex, true, false, alTogether * glm::translate(identity, glm::vec3(3.0f, -0.5f, -6.0f - i * 15.0f)));
	}

	scale = glm::scale(identity, glm::vec3(1.0f, 1.5f, 1.0f));
	rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rotateMosque = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < numOfBlockComponent; i++) {
		if (i == 0) {
			component.waterTank(shaderMP, false, alTogether * glm::translate(identity, glm::vec3(-3.0f, -0.5f, 7.0f - 4.5f * i)));
			component.waterTank(shaderMP, false, alTogether * glm::translate(identity, glm::vec3(4.0f, -0.5f, 7.0f - 4.5f * i)));
		}
		else if (i == 3) {
			component.mosque(shaderTex, shaderMP, alTogether * glm::translate(identity, glm::vec3(-3.7f, -0.5f, 7.0f - 4.0f * i)) * rotate * scale);
			component.building(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(3.7f, -0.5f, 7.0f - 4.0f * i)) * rotate * scale);
		}
		else if (i == 1) {
			component.building(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(-3.7f, -0.5f, 7.0f - 4.0f * i)) * rotate * scale);
			component.mosque(shaderTex, shaderMP, alTogether * glm::translate(identity, glm::vec3(5.7f, -0.5f, 7.0f - 6.0f * i)) * rotateMosque * rotate * scale);
		}
		else {
			component.building(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(-3.7f, -0.5f, 7.0f - 4.0f * i)) * rotate * scale);
			component.building(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(3.7f, -0.5f, 7.0f - 4.0f * i)) * rotate * scale);
		}

	}

	for (int i = 0; i < 2; i++) {
		if (i == 0) {
			world.hills(shaderTex, shaderCurves, false, alTogether * glm::translate(identity, glm::vec3(10.7f, -0.5f, 0.0f - 15.0f * i)) * glm::scale(identity, glm::vec3(2.0f, 3.0f, 2.0f)));
			world.hillSinglePeak(shaderTex, shaderCurves, false, alTogether * glm::translate(identity, glm::vec3(-8.7f, -0.5f, 0.0f - 15.0f * i)) * glm::scale(identity, glm::vec3(2.0f, 3.0f, 1.5f)));
		}
		else {
			world.hillSinglePeak(shaderTex, shaderCurves, false, alTogether * glm::translate(identity, glm::vec3(10.7f, -0.5f, 0.0f - 15.0f * i)) * glm::scale(identity, glm::vec3(1.0f, 3.0f, 1.5f)));
			world.hills(shaderTex, shaderCurves, false, alTogether * glm::translate(identity, glm::vec3(-8.7f, -0.5f, 0.0f - 15.0f * i)) * glm::scale(identity, glm::vec3(1.0f, 3.0f, 2.0f)));
		}
		world.residential(shaderTex, true, true, alTogether * glm::translate(identity, glm::vec3(-6.0f, -0.5f, -6.0f - i * 15.0f)));
		world.residential(shaderTex, true, false, alTogether * glm::translate(identity, glm::vec3(6.0f, -0.5f, -6.0f - i * 15.0f)));

	}

	shaderMP.use();

	world.road(shaderMP, alTogether * glm::translate(identity, glm::vec3(-1.0f, -0.5f, -21.0f)));
	rotate = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (int i = 0; i < numOfStreetLight; i++) {
		// left side of the road
		component.streetlight(shaderMP, lightCubeShader, alTogether * glm::translate(identity, glm::vec3(-1.0f, -0.5f, 5.0f - i * 4.0f)));
		// right side of the road
		component.streetlight(shaderMP, lightCubeShader, alTogether * glm::translate(identity, glm::vec3(3.0f, -0.5f, 5.0f - i * 4.0f)) * rotate);
	}
	for (int i = 0; i < numOfStreetLight; i++) {
		// left side of the road
		component.tree(shaderMP, shaderCurves, false, alTogether * glm::translate(identity, glm::vec3(-1.0f, -0.5f, 12.0f - i * 4.0f)));
		// right side of the road
		component.tree(shaderMP, shaderCurves, false, alTogether * glm::translate(identity, glm::vec3(3.0f, -0.5f, 12.0f - i * 4.0f)) * rotate);
	}
}

void protagonistMoveManager(Character& protagonist, Shader& shaderMP, Shader& lightCubeShader, glm::mat4 revolve)
{
	glm::mat4 rotate, scale, translate, identity = glm::mat4(1.0f), protagonistInitial, protagonistMove, protagonistAlTogether, antagonistMove, antagonistAlTogether;

	rotate = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scale = glm::scale(identity, glm::vec3(0.5f, 0.5f, 0.5f));
	protagonistInitial = glm::translate(identity, glm::vec3(protagonistXinitial, protagonistYinitial, protagonistZinitial));
	protagonistMove = glm::translate(identity, glm::vec3(protagonistXmove, protagonistYmove, protagonistZmove));
	protagonistAlTogether = protagonistMove * protagonistInitial * rotate * scale * revolve;
	if (bigRobotCameo == false) antagonistMove = glm::translate(identity, glm::vec3(protagonistXmove, protagonistYmove, protagonistZmove + 6.0f));
	else {
		antagonistMove = glm::translate(identity, glm::vec3(protagonistXmove + 0.5f, protagonistYmove, protagonistZmove + 0.5f));
	}
	antagonistAlTogether = antagonistMove * protagonistInitial * rotate * scale * revolve;

	float fcoins = coins / 20.0f;
	float ffuel = fuel / 20.0f;
	float fboost = boostBonusEffectDuration / (maxBoostBonusEffectDuration * 1.0f);
	if (protagonistMovementForm == 0) {
		protagonist.drawProtagonist(shaderMP, lightCubeShader, protagonistAlTogether, "still", fcoins, ffuel, fboost);
		if (!gameWon) protagonist.drawAntagonist(shaderMP, lightCubeShader, antagonistAlTogether, "still");
	}
	else if (protagonistMovementForm == 1) {
		protagonist.drawProtagonist(shaderMP, lightCubeShader, protagonistAlTogether, "right", fcoins, ffuel, fboost);
		if (!gameWon) protagonist.drawAntagonist(shaderMP, lightCubeShader, antagonistAlTogether, "right");
	}
	else if (protagonistMovementForm == 2) {
		protagonist.drawProtagonist(shaderMP, lightCubeShader, protagonistAlTogether, "left", fcoins, ffuel, fboost);
		if (!gameWon) protagonist.drawAntagonist(shaderMP, lightCubeShader, antagonistAlTogether, "left");
	}

	if (jumpCoolDown == 15) protagonistYmove = 0.0f;

	if (protagonistMovementForm != 0) protagonistMovementFormCounter++;
	if (protagonistMovementFormCounter > 20 && protagonistMovementFormCounter < 40) protagonistMovementForm = 2;
	else if (protagonistMovementFormCounter > 40) {
		protagonistMovementForm = 0;
		protagonistMovementFormCounter = 0;
	}
}

void collectorItemsManager(Shader& shader1, Shader& shader2, CollectorItems& items, World& world, Components& component)
{
	glm::mat4 rotate, translate, scale, model, identity = glm::mat4(1.0f);
	rotate = glm::rotate(identity, glm::radians(itemAngleChange), glm::vec3(0.0f, 1.0f, 0.0f));
	float firstItemZ = protagonistZinitial - 3.0f;

	// 0 = boost, 1 = coin, 2 = fuel, 3 = dark, 4 = slow
	int numOfBonusItems = bonusItemSequence.size();

	for (int i = 0; i < numOfBonusItems; i++) {
		if (protagonistZcurrent >= bonusItemZPositions[i] - .05f && protagonistZcurrent <= bonusItemZPositions[i] + 0.05f) {
			if (protagonistXcurrent >= bonusItemXPositions[i] - .2f && protagonistXcurrent <= bonusItemXPositions[i] + 0.2f) {
				bonusItemZPositions[i] -= bonusItemOffset;
				if (!jumping) {
					bonusItemZPositions[i] -= bonusItemOffset;
					if (bonusItemSequence[i] == 0) {
						if (!boostBonusAchieved) {
							boostBonusAchieved = true;
							slowBonusAchieved = false;
						}
					}
					if (bonusItemSequence[i] == 1) coins += (10 * boostFactor);
					if (bonusItemSequence[i] == 2) fuel += (5 * boostFactor);
					if (bonusItemSequence[i] == 3) {
						if (!darkBonusAchieved) {
							darkBonusAchieved = true;
							fuel -= 1;
						}
					}
					if (bonusItemSequence[i] == 4) {
						if (!slowBonusAchieved && !boostBonusAchieved) slowBonusAchieved = true;
					}
				}
			}
		}
		else if (camera.Position.z < bonusItemZPositions[i]) {
			bonusItemZPositions[i] -= bonusItemOffset;
		}
	}

	for (int i = 0; i < numOfBonusItems; i++) {
		if (bonusItemSequence[i] == 0) items.boostBonusItem(shader1, shader2, 2, glm::translate(identity, glm::vec3(bonusItemXPositions[i], bonusItemYPositions[i], bonusItemZPositions[i])) * rotate);
		else if (bonusItemSequence[i] == 1) items.coinBonusItem(shader1, shader2, 2, glm::translate(identity, glm::vec3(bonusItemXPositions[i], bonusItemYPositions[i], bonusItemZPositions[i])) * rotate);
		else if (bonusItemSequence[i] == 2) items.fuelBonusItem(shader1, shader2, 2, glm::translate(identity, glm::vec3(bonusItemXPositions[i], bonusItemYPositions[i], bonusItemZPositions[i])) * rotate);
		else if (bonusItemSequence[i] == 3) items.darkBonusItem(shader1, shader2, 2, glm::translate(identity, glm::vec3(bonusItemXPositions[i], bonusItemYPositions[i], bonusItemZPositions[i])) * rotate);
		else items.slowBonusItem(shader1, shader2, 2, glm::translate(identity, glm::vec3(bonusItemXPositions[i], bonusItemYPositions[i], bonusItemZPositions[i])) * rotate);
	}

	itemAngleChange += 1.0f;
	if (itemAngleChange > 360) itemAngleChange = 1;

	bonusManager();
}

void bonusManager()
{
	if (boostBonusAchieved) {
		boostBonusEffectDuration++;
		if (boostFactor < 2) boostFactor++;
		else boostFactor = 2;
		if (boostBonusEffectDuration > maxBoostBonusEffectDuration) {
			boostBonusAchieved = false;
			boostBonusEffectDuration = 0;
			boostFactor = 1;
		}
	}

	if (slowBonusAchieved) {
		slowBonusEffectDuration++;
		boostFactor = 0;
		if (slowBonusEffectDuration > maxSlowBonusEffectDuration) {
			slowBonusAchieved = false;
			slowBonusEffectDuration = 0;
			boostFactor = 1;
		}
	}

	if (darkBonusAchieved) {
		darkBonusEffectDuration++;
		if (darkBonusEffectDuration > maxDarkBonusEffectDuration) {
			darkBonusAchieved = false;
			darkBonusEffectDuration = 0;
		}
	}
}

void gameFreezeManager(Shader& shaderTex, Shader& shaderMP, Components& component, Character& antagonist)
{
	glm::mat4 identity = glm::mat4(1.0f);

	camera.ResetPosition();

	darkBonusAchieved = false;
	dayNightSystem = false;

	if (!gameFinished) {
		component.startMsg(shaderTex, identity * glm::translate(identity, glm::vec3(0.0f, 0.0f, protagonistZinitial - 2.0f)));
	}
	else if (gameWon == true) {
		component.winMsg(shaderTex, identity * glm::translate(identity, glm::vec3(0.0f, 0.0f, protagonistZmove - 0.2f)));
	}
	else if (gameLost == true) {
		component.loseMsg(shaderTex, identity * glm::translate(identity, glm::vec3(0.0f, 0.0f, protagonistZmove - 0.2f)));
	}
}

void writeWelcomMsgCmd()
{
	cout << "<====================== ESCAPE GAME ======================>\n";
	cout << "\n";
	cout << "Welcome !\n";
	cout << "You have a robot. You need it to survive for 1 minute to escape from the big robot behind you and win. You will lose if - \n";
	cout << "1. You stand still for too long.\n";
	cout << "2. Your fuel becomes zero. Fuel - \n";
	cout << "	a)	Refuels if you collect blue items.\n";
	cout << "	b)	Drowns out each frame automatically.\n";
	cout << "	c)	Once it gets too low, coins are used to refuel fuel until coins become zero.\n";
	cout << "\n";
	cout << "<--- Bonus Items --->\n";
	cout << "\n";
	cout << "Green item		: Boosts your fuel and coin collection twofold.\n";
	cout << "Gold item		: Gives you coin. If your fuel gets too low, these coins will refill them.\n";
	cout << "Blue item		: Gives you fuel.\n";
	cout << "Red item			: Slows your fuel and coin collection, like a reverse boost.\n";
	cout << "Black item		: Switches off all the light except the torch for a certain time.\n";
	cout << "\n";
	cout << "<--- Controls --->\n";
	cout << "\n";
	cout << "Enter			: Starts the game\n";
	cout << "Escape		: Closes the game\n";
	cout << "W, S, A, D	: Forward, Backward, Left, Right\n";
	cout << "Space			: Jump\n";
	cout << "E, R				: Camera up and down\n";
	cout << "X, Y, Z			: Pitch, Yaw, Roll\n";
	cout << "N				: Nightmode\n";
	cout << "B					: Camera wide view\n";
	cout << "H				: Camera reset\n";
	cout << "T					: Toggle torch\n";
	cout << "\n";
	cout << "<--- Indicators over robot head --->\n";
	cout << "\n";
	cout << "The three bars over robot head indicates coins, fuel and boost duration. As you collect more of them, color will gradually change to green from red. If you lose too much, it will turn bright red.\n";
	cout << "Left gradient bar				: Shows how much coin you have.\n";
	cout << "Middle gradient bar		: Shows how much fuel you have.\n";
	cout << "Right gradient bar			: Shows how much boost duration you have left.\n";
	cout << "\n";
	cout << "<====================== HAVE FUN !! ======================>\n";
}