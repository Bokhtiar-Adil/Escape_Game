#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

using namespace std;

Camera camera(glm::vec3(0.5f, 1.0f, 4.0f));
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
float TABLE_BASE_WIDTH = 0.6f;
float TABLE_LEG_WIDTH = (float)(TABLE_BASE_WIDTH * 0.067);
float TABLE_COLUMN_GAP = 1.0f;
float TABLE_ROW_GAP = 1.2f;
float FLOOR_TILES_WIDTH = TABLE_BASE_WIDTH * 1.2f;
float FLOOR_TILES_GAP = 0.005f;

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

	Shader shader("vsSrc.vs", "fsSrcLightsPhong.fs");
	Shader lightCubeShader("lightCube.vs", "lightCube.fs");

	// vertices and buffers

	//float vertices[] = {
	//	0.0f, 0.0f, 0.0f,  // 0
	//	1.0f, 0.0f, 0.0f,  // 1
	//	1.0f, 0.0f, -1.0f, // 2
	//	0.0f, 0.0f, -1.0f,  // 3
	//	0.0f, 1.0f, 0.0f,  // 4
	//	1.0f, 1.0f, 0.0f,  //5
	//	1.0f, 1.0f, -1.0f,  // 6
	//	0.0f, 1.0f, -1.0f,  // 7
	//};

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
		4, 5, 7,
		5, 6, 7,
		0, 1, 5,
		0, 5, 4,
		1, 2, 6,
		1, 6, 5,
		3, 2, 6,
		3, 6, 7,
		0, 3, 7,
		0, 7, 4,
	};


	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); 

	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); 
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); 
	glEnableVertexAttribArray(2); 

	glBindVertexArray(0);

	unsigned int lightCubeVAO; 
	glGenVertexArrays(1, &lightCubeVAO); 
	glBindVertexArray(lightCubeVAO); 

	glBindBuffer(GL_ARRAY_BUFFER, VBO); 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); 
	glEnableVertexAttribArray(0); 

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	// render

	glm::mat4 offset, altogether, projection, view, translate, rotate, scale, identity = glm::mat4(1.0f);
	float xoffset = 0.0f, yoffset = 0.0f, zoffset = 0.0f;

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

		shader.use(); 
		shader.setMat4("projection", projection); 
		shader.setMat4("view", view); 
		shader.setBool("nightMode", false);
		shader.setBool("flashlightOn", false);		
		shader.setInt("numberofPointlights", 0);		
		shader.setVec3("viewPos", camera.Position);

		// light properties
		
		// directional light
		shader.setVec3("dirLight.direction", -2.5f, -3.0f, -3.0f);
		shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		shader.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
		shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setBool("nightMode", nightMode);

		// spotlight
		shader.setVec3("spotLight.position", camera.Position);
		shader.setVec3("spotLight.direction", camera.Front);
		shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.09f);
		shader.setFloat("spotLight.quadratic", 0.032f);
		shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		shader.setBool("flashlightOn", torchOn);

		// light properties control
		shader.setBool("ambientOn", ambientOn);
		shader.setBool("diffuseOn", diffuseOn);
		shader.setBool("specularOn", specularOn);

		xoffset = 0.5f, yoffset = 0.5f;
		offset = glm::translate(identity, glm::vec3(xoffset, yoffset, zoffset));
		//shader.setInt("numberofPointlights", 0);
		shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));
		shader.setInt("numberofPointlights", numOfPointLightRoad);
		road(VAO, shader, offset);	
		shader.setInt("numberofPointlights", numOfPointLightRoom);
		simpleRoom(VAO, shader, offset, glm::mat4(1.0f), lightCubeVAO, lightCubeShader);
		/*shader.setInt("numberofPointlights", 0);
		shader.setBool("exposedToSun", true);		*/
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	shader.deleteProgram();

	glfwTerminate();
	return 0;
}



void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 model = glm::mat4(1.0f))
{
	shader.use();
	shader.setMat4("model", model);
	glBindVertexArray(cubeVAO);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void roadsideBlocks(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{
	float width = 5.0f;
	float length = 7.0f;
	glm::mat4 identity, model, translate, scale;
	identity = glm::mat4(1.0f);
	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", false);
	shader.setVec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVec3("material.diffuse", glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("emission", glm::vec3(0.0f, 0.1f, 0.0f));
	
	scale = glm::scale(identity, glm::vec3(-width, 0.1f, length));
	model = alTogether * scale * offset;
	drawCube(VAO, shader, model);
}

void road(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{
	float roadWidth = 2.0f;
	float roadLength = 7.0f;

	glm::mat4 identity, model, translate, scale;
	identity = glm::mat4(1.0f);

	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", true);
	unsigned int diffuseMap = loadTexture("roadLined.jpg");
	unsigned int specularMap = loadTexture("roadLined.jpg");	
	shader.setInt("materialtex.diffuse", 0);
	shader.setInt("materialtex.specular", 1);
	shader.setFloat("materialtex.shininess", 32.0f);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, specularMap); 

	scale = glm::scale(identity, glm::vec3(roadWidth, 0.1f, roadLength));
	//translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1 * roadLength));
	model = alTogether * scale * offset;

	shader.setMat4("model", model);
	drawCube(VAO, shader, model);

	float xoffset = 0.5f, yoffset = 0.5f;
	box(VAO, shader, glm::translate(identity, glm::vec3(5.0f, yoffset + 0.2f, -1.0f)));
	box(VAO, shader, glm::translate(identity, glm::vec3(5.0f, yoffset + 1.2f, -1.0f)));
	box(VAO, shader, glm::translate(identity, glm::vec3(5.0f, yoffset + 0.2f, 0.0f)));
	roadsideBlocks(VAO, shader, glm::translate(identity, glm::vec3(xoffset, yoffset - 0.01f, 0.0f)), alTogether);
	roadsideBlocks(VAO, shader, glm::translate(identity, glm::vec3(xoffset, yoffset - 0.01f, 0.0f)), glm::translate(identity, glm::vec3(5.01f, 0.0f, 0.0f)));
	shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));

	// lamp
	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", false);
	shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("material.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));
	scale = glm::scale(identity, glm::vec3(0.05f, 1.5f, 0.05f));
	translate = glm::translate(identity, glm::vec3(2.0f, 0.0f, 0.0f));
	model = translate * scale * offset;;
	glm::mat4 modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.0f));
	model = translate * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 4.0f));
	model = translate * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);

	shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	scale = glm::scale(identity, glm::vec3(0.1f, 0.1f, 0.1f));
	translate = glm::translate(identity, glm::vec3(1.9f, 1.4f, 0.0f));
	model = translate * scale * offset;;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.0f));
	model = translate * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 4.0f));
	model = translate * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);

	shader.setVec3("pointLights[0].position", glm::vec3(1.9f, 1.5f, 2.1f));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	shader.setBool("pointLightStatus[0]", true);

	shader.setVec3("pointLights[1].position", glm::vec3(2.0f, 1.5f, 0.15f));
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);
	shader.setBool("pointLightStatus[1]", true);

	shader.setVec3("pointLights[2].position", glm::vec3(2.0f, 1.5f, -1.95f));
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09f);
	shader.setFloat("pointLights[2].quadratic", 0.032f);
	shader.setBool("pointLightStatus[2]", true);
}

void table(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{
	float baseWidth = 1.5f;
	float baseHeight = 0.1f;
	float gap = 0.2f;

	glm::mat4 identity, model, modelTogether, scale, translate, rotate;
	identity = glm::mat4(1.0f);

	shader.setBool("withTexture", true);
	unsigned int diffuseMap = loadTexture("woodenSurface.jpg");
	unsigned int specularMap = loadTexture("woodenSurface.jpg");
	shader.setInt("materialtex.diffuse", 0);
	shader.setInt("materialtex.specular", 1);
	shader.setFloat("materialtex.shininess", 32.0f);
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, diffuseMap); 
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, specularMap);

	model = offset;
	scale = glm::scale(identity, glm::vec3(baseWidth, baseHeight, baseWidth));
	translate = glm::translate(identity, glm::vec3(3.5f, 0.0f, -2.0f));
	model =  translate * scale * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);

	scale = glm::scale(identity, glm::vec3(baseWidth/3.0f, 4.0f, 0.2f));
	translate = glm::translate(identity, glm::vec3(2.1f, -0.4f, -1.0f));
	model = translate * scale * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);

	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1.2f));
	model = translate * model;
	modelTogether = alTogether * model;
	drawCube(VAO, shader, modelTogether);
}

void box(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{
	float boxWidth = 0.5f;
	float boxHeight = 0.5f;
	
	glm::mat4 identity, model, scale, translate;
	identity = glm::mat4(1.0f);

	shader.setBool("withTexture", true);
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	shader.setInt("materialtex.diffuse", 0);
	shader.setInt("materialtex.specular", 1);
	shader.setFloat("materialtex.shininess", 32.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	scale = glm::scale(identity, glm::vec3(boxWidth, boxHeight, boxWidth));
	//translate = glm::translate(identity, glm::vec3(0.5f, 0.5f, 0.0f));
	model = alTogether * scale * offset;
	drawCube(VAO, shader, model);
}

void simpleRoom(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether, unsigned int& lightCubeVAO, Shader& lightCubeShader)
{
	float crWidth = 5.0f;
	float crLength = 5.0f;
	float crHeight = 2.0f;
	float numTables = 16;

	glm::mat4 identity, model, translate, scale, rotate;
	identity = glm::mat4(1.0f);

	shader.setBool("withTexture", true);
	shader.setInt("materialtex.diffuse", 0);
	shader.setInt("materialtex.specular", 1);
	shader.setFloat("materialtex.shininess", 32.0f);	

	// pointlight src inside simpleRoom
	scale = glm::scale(identity, glm::vec3(0.5f, 0.05f, 0.05f));
	translate = glm::translate(identity, glm::vec3(-2.7f, 1.8f, -0.1f));
	glm::mat4 lightCube =  translate * scale * offset;
	lightCubeShader.use();
	if (numOfPointLightRoom == 1) lightCubeShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
	if (numOfPointLightRoom == 0) lightCubeShader.setVec3("color", glm::vec3(0.1f, 0.1f, 0.1f));
	drawCube(lightCubeVAO, lightCubeShader, lightCube); 

	shader.use();
	//shader.setInt("numberofPointlights", 1);
	shader.setVec3("pointLights[0].position", glm::vec3(-2.4f, 1.8f, 0.1f));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	shader.setBool("pointLightStatus[0]", true);

	// floor

	unsigned int diffuseMap = loadTexture("designedTiles.jpg");
	unsigned int specularMap = loadTexture("designedTiles.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	
	shader.setBool("exposedToSun", false);
	scale = glm::scale(identity, glm::vec3(-crWidth, 0.1f, crLength));	
	model = alTogether * scale * offset;
	drawCube(VAO, shader, model);

	diffuseMap = loadTexture("whitewall2.jpg");
	specularMap = loadTexture("whitewall2.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	//ceilling
	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", false);
	shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 model2 = model;
	translate = glm::translate(identity, glm::vec3(0.0f, crHeight-0.1f, 0.0f));
	model2 = alTogether * translate * model2;
	drawCube(VAO, shader, model2);

	// walls
	// roadside wall of 1st simpleRoom
	shader.setBool("exposedToSun", true);
	shader.setInt("numberofPointlights", numOfPointLightRoad);
	shader.setVec3("pointLights[0].position", glm::vec3(1.9f, 1.5f, 2.1f));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	shader.setBool("pointLightStatus[0]", true);

	shader.setVec3("pointLights[1].position", glm::vec3(2.0f, 1.5f, 0.15f));
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);
	shader.setBool("pointLightStatus[1]", true);

	shader.setVec3("pointLights[2].position", glm::vec3(2.0f, 1.5f, -1.95f));
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09f);
	shader.setFloat("pointLights[2].quadratic", 0.032f);
	shader.setBool("pointLightStatus[2]", true);
	
	rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scale = glm::scale(identity, glm::vec3(1.0f, 0.4f, 1.0f));
	translate = glm::translate(identity, glm::vec3(-0.1f, 0.0f, 0.0f));
	model = alTogether * translate * scale * rotate * model;
	drawCube(VAO, shader, model);	
	shader.setBool("exposedToSun", false);
	shader.setInt("numberofPointlights", numOfPointLightRoom);
	shader.setVec3("pointLights[0].position", glm::vec3(-2.4f, 1.8f, 0.1f));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	shader.setBool("pointLightStatus[0]", true);

	translate = glm::translate(identity, glm::vec3(-0.05f, 0.0f, 0.0f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	model = glm::translate(model, glm::vec3(-0.05f, 0.0f, 0.0f));
	glm::mat4 door = model;
	

	// opposite to road side of 1st simpleRoom
	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", false);
	shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));
	translate = glm::translate(identity, glm::vec3(-crWidth + 0.1f, 0.0f, 0.0f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	shader.setBool("exposedToSun", false);
	shader.setBool("withTexture", true);
	translate = glm::translate(identity, glm::vec3(0.05f, 0.0f, 0.0f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	model = glm::translate(model, glm::vec3(-0.05f, 0.0f, 0.0f));

	 // farside wall of 1st simpleRoom
	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", false);
	shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("emission", glm::vec3(0.1f, 0.1f, 0.1f));
	rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	translate = glm::translate(identity, glm::vec3(-crWidth/2.0f, 0.1f, crWidth / 2.0f));
	model = alTogether * translate * rotate * model;
	drawCube(VAO, shader, model);
	shader.setBool("exposedToSun", false);
	shader.setBool("withTexture", true);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.05f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.05f));
	glm::mat4 frame = model;

	// nearside wall of 1st simpleRoom
	shader.setBool("exposedToSun", true);
	translate = glm::translate(identity, glm::vec3(-0.25f, 0.0f, crWidth-0.14));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	shader.setBool("exposedToSun", false);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -0.05f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);

	diffuseMap = loadTexture("palestine.jpg");
	specularMap = loadTexture("palestine.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scale = glm::scale(identity, glm::vec3(0.5f, 0.2f, 0.5f));
	translate = glm::translate(identity, glm::vec3(-1.5f, 0.9f, -1.1f));
	frame = translate * scale * rotate * frame;
	drawCube(VAO, shader, frame);

	alTogether = glm::translate(identity, glm::vec3(-crWidth*2.0f + 2.5f, 0.4f, 1.0f)); 
	table(VAO, shader, offset, alTogether);
	alTogether = glm::translate(alTogether, glm::vec3(0.0f, 0.0f, 2.0f));
	table(VAO, shader, offset, alTogether);

	shader.setBool("exposedToSun", true);
	shader.setBool("withTexture", true);
	/*shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setFloat("material.shininess", 32.0f);
	shader.setVec3("emission", glm::vec3(0.1f, 0.0f, 0.0f));*/
	diffuseMap = loadTexture("woodCypress.jpg");
	specularMap = loadTexture("woodCypress.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	shader.setBool("exposedToSun", true);
	shader.setInt("numberofPointlights", numOfPointLightRoad);
	shader.setVec3("pointLights[0].position", glm::vec3(1.9f, 1.5f, 2.1f));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	shader.setBool("pointLightStatus[0]", true);

	shader.setVec3("pointLights[1].position", glm::vec3(2.0f, 1.5f, 0.15f));
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);
	shader.setBool("pointLightStatus[1]", true);

	shader.setVec3("pointLights[2].position", glm::vec3(2.0f, 1.5f, -1.95f));
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09f);
	shader.setFloat("pointLights[2].quadratic", 0.032f);
	shader.setBool("pointLightStatus[2]", true);

	scale = glm::scale(identity, glm::vec3(1.0f, 0.6f, 0.2f));
	translate = glm::translate(identity, glm::vec3(crWidth+2.5f+0.06f, -0.2f, -1.2f));
	door = alTogether * translate * scale * door;
	drawCube(VAO, shader, door);
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
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		ambientOn = !ambientOn;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		diffuseOn = !diffuseOn;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		specularOn = !specularOn;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		ambientOn = true;
		diffuseOn = true;
		specularOn = true;
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

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}