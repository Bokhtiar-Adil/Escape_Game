#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "Components.h"
#include "World.h"
#include "Character.h"

using namespace std;

float initialCameraZ = 8.0f;
Camera camera(glm::vec3(1.0f, 0.2f, initialCameraZ));
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

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
//unsigned int loadTexture(char const* path);
//void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 model);

unsigned int dummyVAO = NULL;
Shader dummyShader;

// components
void road(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void table(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void box(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void simpleRoom(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f), unsigned int& lightCubeVAO = dummyVAO, Shader& lightCubeShader = dummyShader);


// helper functions
void shaderSetup(Shader& lightCubeShader, Shader& shaderTex, Shader& shaderMP, glm::mat4& projection, glm::mat4& view);
void worldExpansion(Shader& shaderTex, Shader& shaderMP, World& world, Components& component, glm::mat4 revolve);
void protagonistMoveHandler(Character& protagonist, Shader& shaderMP, glm::mat4 revolve);
void streetlightSetup(Shader& shader, float moveZ, float slAmb = 0.1f, float slDiff = 0.5f, float slSpec = 0.5f, float slConst = 1.0f, float slLin = 0.09f, float slQuad = 0.032f);
void dayNightControl();

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
float protagonistZmove = 0.0f, protagonistXmove = 0.0f, protagonistYmove = 0.0f;
float protagonistXinitial = 1.05f, protagonistYinitial = 0.4f, protagonistZinitial = 5.0f;
int jumpCoolDown = 0;
float sunX = 1.05f;
float sunY = 3.0f;
float sunZ = 5.0f;
int nightDuration = 0;
float sunAmb = 0.3f, sunDiff = 0.8f, sunSpec = 1.0f;
bool dayNightCycleMode = true;
bool dayNightDirectMode = false;

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

	Shader shaderMP("vsSrc.vs", "fsSrcMatProp.fs");
	Shader shaderTex("vsSrc.vs", "fsSrcTex.fs");
	Shader shader("vsSrc.vs", "fsSrcBoth.fs");
	Shader lightCubeShader("lightCube.vs", "lightCube.fs");

	glEnable(GL_DEPTH_TEST);

	// render

	glm::mat4 offset, altogether, projection, view, translate, rotate, scale, identity = glm::mat4(1.0f);
	float xoffset = 0.0f, yoffset = 0.0f, zoffset = 0.0f;

	Components component;
	World world;
	Character protagonist;

	float px = 1.0f, py = 0.2f, pz = initialCameraZ;


	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		jumpCoolDown++;

		processInput(window);

		if (!nightMode) glClearColor(sunDiff+0.2f, sunDiff + 0.2f, sunDiff + 0.2f, sunDiff + 0.2f);
		else glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 30.0f);
		view = camera.GetViewMatrix();

		shaderSetup(lightCubeShader, shaderTex, shaderMP, projection, view);
		shaderTex.use();
		streetlightSetup(shaderTex, protagonistZmove);
		shaderMP.use();
		streetlightSetup(shaderMP, protagonistZmove);
		
		xoffset = 0.5f, yoffset = 0.5f;
		offset = glm::translate(identity, glm::vec3(xoffset, yoffset, zoffset));
		
		glm::mat4 rotateXMatrix = glm::rotate(identity, glm::radians(rotate_obj_x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotateYMatrix = glm::rotate(identity, glm::radians(rotate_obj_y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotateZMatrix = glm::rotate(identity, glm::radians(rotate_obj_z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 revolve = rotateZMatrix * rotateYMatrix * rotateXMatrix;

		currentBlockBase = 0.0f;
		if ((-1 * camera.Position.z + initialCameraZ) - currentBlockNumber * 30.0f > 30.0f) {
			currentBlockNumber += 1;
		}
		currentBlockBase = -1 * (currentBlockNumber * 30.0f);
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, currentBlockBase + 0.0f)); 
		worldExpansion(shaderTex, shaderMP, world, component, translate);
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, currentBlockBase - 30.0f));
		worldExpansion(shaderTex, shaderMP, world, component, translate);
		/**/

		protagonistMoveHandler(protagonist, shaderMP, revolve);
		
		//component.tree(shaderMP, false, glm::translate(identity, glm::vec3(0.0f, 0.0f, 3.0f)) * revolve);

		//if (camera.Position.x != px) cout << "cam x: " << camera.Position.x << "\n";
		//if (camera.Position.y != py) cout << "cam y: " << camera.Position.y << "\n";
		//if (camera.Position.z != pz) cout << "cam z: " << camera.Position.z << "\n";
		px = camera.Position.x;
		py = camera.Position.y;
		pz = camera.Position.z;

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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		currentCharacterPos += 0.05f;
		protagonistZmove -= (camera.MovementSpeed * deltaTime);
	}
		
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		currentCharacterPos -= 0.05f;
		protagonistZmove += (camera.MovementSpeed * deltaTime);
	}
		
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		protagonistXmove -= (camera.MovementSpeed * deltaTime);
	}
		
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		protagonistXmove += (camera.MovementSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		currentCharacterPos += 0.05f;
		protagonistZmove -= (camera.MovementSpeed * deltaTime);
		if (jumpCoolDown >= 20) {
			protagonistYmove = (25.0f * camera.MovementSpeed * deltaTime);
			jumpCoolDown = 0;
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
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		torchOn = !torchOn;
	}
	 // nightmode contol
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {		
		if (nightMode == false) {
			dayNightCycleMode = false;
			nightMode = true;
			/*sunAmb = 0.0f;
			sunDiff = 0.0f;
			sunSpec = 1.0f;*/
			dayNightDirectMode = true;
			dayNightControl();
		}
		else {
			/*dayNightCycleMode = true;*/
			nightMode = false;
			/*sunAmb = 0.3f;
			sunDiff = 0.8f;
			sunSpec = 1.0f;*/
			dayNightDirectMode = false;
			dayNightControl();
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
		dayNightCycleMode = !dayNightCycleMode;
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
	if (dayNightCycleMode) {
		float gradualSunLightChange = 0.001f;
		float gradualSunPositionChange = 0.01f;
		if (sunX > -6.0f) {
			if (sunAmb < 0.3f) sunAmb += gradualSunLightChange;
			if (sunDiff < 0.8f) sunDiff += gradualSunLightChange;
			if (sunSpec < 1.0f) sunSpec += gradualSunLightChange;
			if (sunAmb >= 0.3f && sunDiff >= 0.8f && sunSpec >= 1.0f) sunX -= gradualSunPositionChange;
		}
		else {
			if (sunAmb > 0.0f) sunAmb -= gradualSunLightChange;
			if (sunDiff > 0.0f) sunDiff -= gradualSunLightChange;
			if (sunSpec > 0.0f) sunSpec -= gradualSunLightChange;
			//sunX = 6.0f;
			if (sunDiff <= 0.0f) nightMode = true;
			if (nightMode) nightDuration++;
			if (nightDuration == 1000) {
				nightMode = false;
				nightDuration = 0;
				sunX = 6.0f;
			}
		}
		if (sunDiff <= 0.3f) streetLightOn = true;
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

void shaderSetup(Shader& lightCubeShader, Shader& shaderTex, Shader& shaderMP, glm::mat4& projection, glm::mat4& view)
{
	lightCubeShader.use();
	lightCubeShader.setMat4("projection", projection);
	lightCubeShader.setMat4("view", view);
	
	shaderTex.use();
	shaderTex.setMat4("projection", projection);
	shaderTex.setMat4("view", view);
	if (nightMode) shaderTex.setBool("nightMode", true);
	else shaderTex.setBool("nightMode", false);
	shaderTex.setBool("flashlightOn", false);
	shaderTex.setInt("numberofPointlights", 0);
	shaderTex.setVec3("viewPos", camera.Position);

	// light properties
	dayNightControl();
	//if (dayNightCycleMode) {
	//	float gradualSunLightChange = 0.001f;
	//	float gradualSunPositionChange = 0.01f;
	//	if (sunX > -6.0f) {
	//		if (sunAmb < 0.3f) sunAmb += gradualSunLightChange;
	//		if (sunDiff < 0.8f) sunDiff += gradualSunLightChange;
	//		if (sunSpec < 1.0f) sunSpec += gradualSunLightChange;
	//		if (sunAmb >= 0.3f && sunDiff >= 0.8f && sunSpec >= 1.0f) sunX -= gradualSunPositionChange;
	//	}
	//	else {
	//		if (sunAmb > 0.0f) sunAmb -= gradualSunLightChange;
	//		if (sunDiff > 0.0f) sunDiff -= gradualSunLightChange;
	//		if (sunSpec > 0.0f) sunSpec -= gradualSunLightChange;
	//		//sunX = 6.0f;
	//		if (sunDiff <= 0.0f) nightMode = true;
	//		if (nightMode) nightDuration++;
	//		if (nightDuration == 1000) {
	//			nightMode = false;
	//			nightDuration = 0;
	//			sunX = 6.0f;
	//		}
	//	}
	//}
	//else {
	//	if (!dayNightDirectMode) {
	//		sunX = 1.05f;
	//		sunY = 3.0f;
	//		sunZ = 5.0f;
	//		nightDuration = 0;
	//		sunAmb = 0.3f;
	//		sunDiff = 0.8f;
	//		sunSpec = 1.0f;
	//		nightMode = false;
	//	}
	//	
	//}

	// directional light
	/*shaderTex.setVec3("dirLight.direction", 1.0f, -3.0f, -3.0f);
	shaderTex.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
	shaderTex.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	shaderTex.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
	shaderTex.setBool("nightMode", nightMode);*/

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

	shaderTex.setInt("numberofStreetlights", 14);
	for (int index = 0; index < 16; index++) {
		shaderTex.setVec3("streetLights[index].ambient", 0.1f, 0.1f, 0.1f);
		shaderTex.setVec3("streetLights[index].diffuse", 1.0f, 1.0f, 1.0f);
		shaderTex.setVec3("streetLights[index].specular", 1.0f, 1.0f, 1.0f);
		shaderTex.setFloat("streetLights[index].constant", 1.0f);
		shaderTex.setFloat("streetLights[index].linear", 0.09f);
		shaderTex.setFloat("streetLights[index].quadratic", 0.032f);
		shaderTex.setBool("streetLightStatus[index]", false);
	}

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

	shaderMP.setInt("numberofStreetlights", 14);
	for (int index = 0; index < 16; index++) {
		shaderMP.setVec3("streetLights[index].ambient", 0.1f, 0.1f, 0.1f);
		shaderMP.setVec3("streetLights[index].diffuse", 1.0f, 1.0f, 1.0f);
		shaderMP.setVec3("streetLights[index].specular", 1.0f, 1.0f, 1.0f);
		shaderMP.setFloat("streetLights[index].constant", 1.0f);
		shaderMP.setFloat("streetLights[index].linear", 0.09f);
		shaderMP.setFloat("streetLights[index].quadratic", 0.032f);
		shaderMP.setBool("streetLightStatus[index]", false);
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

	//float slAmb = 0.1f, slDiff = 0.5f, slSpec = 0.5f, slConst = 1.0f, slLin = 0.09f, slQuad = 0.032f;
	

	shader.use();
	shader.setInt("numberofStreetlights", 14);
	shader.setBool("streetLightOn", streetLightOn);
	shader.setVec3("streetLights[0].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[0].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[0].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[0].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[0].constant", slConst);
	shader.setFloat("streetLights[0].linear", slLin);
	shader.setFloat("streetLights[0].quadratic", slQuad);
	shader.setBool("streetLightStatus[0]", true);
	index++;

	shader.setVec3("streetLights[1].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[1].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[1].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[1].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[1].constant", slConst);
	shader.setFloat("streetLights[1].linear", slLin);
	shader.setFloat("streetLights[1].quadratic", slQuad);
	shader.setBool("streetLightStatus[1]", true);
	index++;

	shader.setVec3("streetLights[2].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[2].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[2].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[2].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[2].constant", slConst);
	shader.setFloat("streetLights[2].linear", slLin);
	shader.setFloat("streetLights[2].quadratic", slQuad);
	shader.setBool("streetLightStatus[2]", true);
	index++;

	shader.setVec3("streetLights[3].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[3].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[3].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[3].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[3].constant", slConst);
	shader.setFloat("streetLights[3].linear", slLin);
	shader.setFloat("streetLights[3].quadratic", slQuad);
	shader.setBool("streetLightStatus[3]", true);
	index++;

	shader.setVec3("streetLights[4].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[4].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[4].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[4].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[4].constant", slConst);
	shader.setFloat("streetLights[4].linear", slLin);
	shader.setFloat("streetLights[4].quadratic", slQuad);
	shader.setBool("streetLightStatus[4]", true);
	index++;

	shader.setVec3("streetLights[5].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[5].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[5].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[5].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[5].constant", slConst);
	shader.setFloat("streetLights[5].linear", slLin);
	shader.setFloat("streetLights[5].quadratic", slQuad);
	shader.setBool("streetLightStatus[5]", true);
	index++;

	shader.setVec3("streetLights[6].position", firstStreetlightXLeft, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[6].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[6].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[6].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[6].constant", slConst);
	shader.setFloat("streetLights[6].linear", slLin);
	shader.setFloat("streetLights[6].quadratic", slQuad);
	shader.setBool("streetLightStatus[6]", true);
	
	index = 0;

	shader.setVec3("streetLights[7].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[7].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[7].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[7].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[7].constant", slConst);
	shader.setFloat("streetLights[7].linear", slLin);
	shader.setFloat("streetLights[7].quadratic", slQuad);
	shader.setBool("streetLightStatus[7]", true);
	index++;

	shader.setVec3("streetLights[8].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[8].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[8].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[8].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[8].constant", slConst);
	shader.setFloat("streetLights[8].linear", slLin);
	shader.setFloat("streetLights[8].quadratic", slQuad);
	shader.setBool("streetLightStatus[8]", true);
	index++;

	shader.setVec3("streetLights[9].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[9].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[9].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[9].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[9].constant", slConst);
	shader.setFloat("streetLights[9].linear", slLin);
	shader.setFloat("streetLights[9].quadratic", slQuad);
	shader.setBool("streetLightStatus[9]", true);
	index++;

	shader.setVec3("streetLights[10].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[10].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[10].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[10].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[10].constant", slConst);
	shader.setFloat("streetLights[10].linear", slLin);
	shader.setFloat("streetLights[10].quadratic", slQuad);
	shader.setBool("streetLightStatus[10]", true);
	index++;

	shader.setVec3("streetLights[11].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[11].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[11].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[11].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[11].constant", slConst);
	shader.setFloat("streetLights[11].linear", slLin);
	shader.setFloat("streetLights[11].quadratic", slQuad);
	shader.setBool("streetLightStatus[11]", true);
	index++;

	shader.setVec3("streetLights[12].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[12].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[12].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[12].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[12].constant", slConst);
	shader.setFloat("streetLights[12].linear", slLin);
	shader.setFloat("streetLights[12].quadratic", slQuad);
	shader.setBool("streetLightStatus[12]", true);
	index++;

	shader.setVec3("streetLights[13].position", firstStreetlightXRight, firstStreetlightY, firstStreetlightZ + index*gap);
	shader.setVec3("streetLights[13].ambient", slAmb, slAmb, slAmb);
	shader.setVec3("streetLights[13].diffuse", slDiff, slDiff, slDiff);
	shader.setVec3("streetLights[13].specular", slSpec, slSpec, slSpec);
	shader.setFloat("streetLights[13].constant", slConst);
	shader.setFloat("streetLights[13].linear", slLin);
	shader.setFloat("streetLights[13].quadratic", slQuad);
	shader.setBool("streetLightStatus[13]", true);
	index++;

}

void worldExpansion(Shader& shaderTex, Shader& shaderMP, World& world, Components& component, glm::mat4 alTogether)
{
	glm::mat4 rotate, scale, translate, identity = glm::mat4(1.0f);
	int numOfBuilding = 7, numOfStreetLight = 7, numOfResidential = 2;

	shaderTex.use();

	for (int i = 0; i < numOfResidential; i++) {
		world.residential(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(-4.0f, -0.5f, -6.0f - i*15.0f)));
		world.residential(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(3.0f, -0.5f, -6.0f - i*15.0f)));
	}	

	scale = glm::scale(identity, glm::vec3(1.0f, 1.5f, 1.0f));
	rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < numOfBuilding; i++) {
		component.building(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(-3.5f, -0.5f, 7.0f - 4.0f*i)) * rotate * scale);
		component.building(shaderTex, true, alTogether * glm::translate(identity, glm::vec3(3.5f, -0.5f, 7.0f - 4.0f*i)) * rotate * scale);		
	}

	shaderMP.use();	

	world.road(shaderMP, alTogether * glm::translate(identity, glm::vec3(-1.0f, -0.5f, -21.0f)));	
	rotate = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (int i = 0; i < numOfStreetLight; i++) {
		// left side of the road
		component.streetlight(shaderMP, alTogether * glm::translate(identity, glm::vec3(-1.0f, -0.5f, 5.0f - i * 4.0f)));
		// right side of the road
		component.streetlight(shaderMP, alTogether * glm::translate(identity, glm::vec3(3.0f, -0.5f, 5.0f - i * 4.0f)) * rotate);
	}
}

void protagonistMoveHandler(Character& protagonist, Shader& shaderMP, glm::mat4 revolve)
{
	glm::mat4 rotate, scale, translate, identity = glm::mat4(1.0f), protagonistInitial, protagonistMove, protagonistAlTogether;

	rotate = glm::rotate(identity, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scale = glm::scale(identity, glm::vec3(0.5f, 0.5f, 0.5f));
	protagonistInitial = glm::translate(identity, glm::vec3(protagonistXinitial, protagonistYinitial, protagonistZinitial));
	protagonistMove = glm::translate(identity, glm::vec3(protagonistXmove, protagonistYmove, protagonistZmove));
	protagonistAlTogether = protagonistMove * protagonistInitial * rotate * scale * revolve;
	protagonist.drawProtagonist(shaderMP, protagonistAlTogether);
	if (jumpCoolDown==15) protagonistYmove = 0.0f;
}