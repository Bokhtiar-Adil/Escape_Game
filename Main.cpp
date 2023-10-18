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

using namespace std;

Camera camera(glm::vec3(0.5f, 0.2f, 4.0f));
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
unsigned int loadTexture(char const* path);
void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 model);

unsigned int dummyVAO = NULL;
Shader dummyShader;

// components
void road(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void table(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void box(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void simpleRoom(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f), unsigned int& lightCubeVAO = dummyVAO, Shader& lightCubeShader = dummyShader);


bool torchOn = false;
bool nightMode = false;
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

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f); 		
		view = camera.GetViewMatrix(); 

		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		shaderTex.use(); 
		shaderTex.setMat4("projection", projection);
		shaderTex.setMat4("view", view);
		shaderTex.setBool("nightMode", false);
		shaderTex.setBool("flashlightOn", false);
		shaderTex.setInt("numberofPointlights", 0);
		shaderTex.setVec3("viewPos", camera.Position);

		// light properties
		
		// directional light
		shaderTex.setVec3("dirLight.direction", -2.5f, -3.0f, -3.0f);
		shaderTex.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		shaderTex.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
		shaderTex.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
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

		

		shaderMP.use(); 
		shaderMP.setMat4("projection", projection); 
		shaderMP.setMat4("view", view); 
		shaderMP.setBool("nightMode", false); 
		shaderMP.setBool("flashlightOn", false); 
		shaderMP.setInt("numberofPointlights", 0); 
		shaderMP.setVec3("viewPos", camera.Position); 

		// light properties

		// directional light 
		shaderMP.setVec3("dirLight.direction", -5.5f, -3.0f, -3.0f); 
		shaderMP.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f); 
		shaderMP.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
		shaderMP.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
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

		xoffset = 0.5f, yoffset = 0.5f;
		offset = glm::translate(identity, glm::vec3(xoffset, yoffset, zoffset));
		//shader.setInt("numberofPointlights", 0);
		/*shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));
		shader.setInt("numberofPointlights", numOfPointLightRoad);
		road(VAO, shader, offset);	
		shader.setInt("numberofPointlights", numOfPointLightRoom);
		simpleRoom(VAO, shader, offset, glm::mat4(1.0f), lightCubeVAO, lightCubeShader);*/
		/*shader.setInt("numberofPointlights", 0);
		shader.setBool("exposedToSun", true);		*/
		
		//table(VAO, shader, offset, glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::mat4 rotateXMatrix = glm::rotate(identity, glm::radians(rotate_obj_x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotateYMatrix = glm::rotate(identity, glm::radians(rotate_obj_y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotateZMatrix = glm::rotate(identity, glm::radians(rotate_obj_z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 revolve = rotateZMatrix * rotateYMatrix * rotateXMatrix; 
		shaderTex.use();
		/*component.table(shaderTex, true); 
		component.chair(shaderTex, true, glm::translate(identity, glm::vec3(0.18f, 0.025f, -0.05f)) * revolve);*/
		//component.building01(shaderTex, true, glm::translate(identity, glm::vec3(-2.0f, 0.0f, -3.0f)) * revolve);
		//component.door_tex(shaderTex, glm::translate(identity, glm::vec3(2.5f, 0.0f, -3.0f))* revolve);
		shaderMP.use();
		/*component.table(shaderMP, false, glm::translate(identity, glm::vec3(2.0f, 0.0f, 0.0f)) * revolve);
		component.chair(shaderMP, false, glm::translate(identity, glm::vec3(2.15f, 0.025f, -0.05f)) * revolve);*/
		// component.building_notex(shaderMP, false, glm::translate(identity, glm::vec3(-1.5f, 0.0f, -3.0f)) * revolve); 
		component.bench(shaderMP, glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f))* revolve);
		/*shader.use();
		component.building_texdoor(shader, glm::translate(identity, glm::vec3(-1.5f, 0.0f, -3.0f))* revolve);*/


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
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
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		nightMode = !nightMode;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		numOfPointLightRoom+=1;
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
		rotate_obj_y += 0.1;
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

//unsigned int loadTexture(char const* path)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}