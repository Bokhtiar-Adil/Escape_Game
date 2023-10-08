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

Camera camera(glm::vec3(0.5f, 0.5f, 4.0f));
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

// components
void road(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void table(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void box(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));
void classroom(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether = glm::mat4(1.0f));


bool torchOn = false;
bool nightMode = false;

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
	//Shader lightCubeShader("light_cube.vs", "light_cube.fs");

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

		shader.use();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f); 
		shader.setMat4("projection", projection);
		view = camera.GetViewMatrix(); 
		shader.setMat4("view", view);		
		
		/*bed(VAO, shader, offset);*/
		/*scale = glm::scale(identity, glm::vec3(0.5f, 0.5f, 0.5f));
		glm::mat4 model = scale * offset;*/
		//drawCube(VAO, shader, model);
		shader.setBool("nightMode", false);
		shader.setBool("flashlightOn", false);
		
		shader.setInt("numberofPointlights", 0);		
		shader.setVec3("viewPos", camera.Position);

		// light properties
		
		// directional light
		shader.setVec3("dirLight.direction", 0.5f, -3.0f, -3.0f);
		shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		shader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
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

		xoffset = 0.5f, yoffset = 0.5f;
		offset = glm::translate(identity, glm::vec3(xoffset, yoffset, zoffset));
		road(VAO, shader, offset);
		classroom(VAO, shader, offset);
		shader.setBool("exposedToSun", true);
		box(VAO, shader, glm::translate(identity, glm::vec3(0.5f, yoffset+0.2f, -1.0f)));
		box(VAO, shader, glm::translate(identity, glm::vec3(3.0f, yoffset+0.2f, -1.0f)));
		

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
	//, float r = 1.0f, float g = 1.0f, float b = 1.0f
	/*shader.setVec3("material.ambient", glm::vec3(r, g, b));
	shader.setVec3("material.diffuse", glm::vec3(r, g, b));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setFloat("material.shininess", 32.0f);*/

	shader.setMat4("model", model);
	glBindVertexArray(cubeVAO);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}



void road(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{
	float roadWidth = 2.0f;
	float roadLength = 20.0f;

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
}

void table(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{

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

void classroom(unsigned int& VAO, Shader& shader, glm::mat4 offset, glm::mat4 alTogether)
{
	float crWidth = 5.0f;
	float crLength = 5.0f;
	float crHeight = 1.5f;
	float numTables = 16;

	glm::mat4 identity, model, translate, scale, rotate;
	identity = glm::mat4(1.0f);

	shader.setBool("withTexture", true);
	shader.setInt("materialtex.diffuse", 0);
	shader.setInt("materialtex.specular", 1);
	shader.setFloat("materialtex.shininess", 32.0f);

	unsigned int diffuseMap = loadTexture("floortiles.jpg");
	unsigned int specularMap = loadTexture("floortiles.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	// floor
	shader.setBool("exposedToSun", false);
	scale = glm::scale(identity, glm::vec3(-crWidth, 0.1f, crLength));	
	model = alTogether * scale * offset;
	drawCube(VAO, shader, model);

	diffuseMap = loadTexture("concrete.png");
	specularMap = loadTexture("concrete.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	//ceilling
	shader.setBool("exposedToSun", true);
	glm::mat4 model2 = model;
	translate = glm::translate(identity, glm::vec3(0.0f, crHeight-0.1f, 0.0f));
	model2 = alTogether * translate * model2;
	drawCube(VAO, shader, model2);

	// walls
	// roadside wall of 1st classroom
	rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scale = glm::scale(identity, glm::vec3(1.0f, 0.3f, 1.0f));
	translate = glm::translate(identity, glm::vec3(-0.1f, 0.0f, 0.0f));
	model = alTogether * translate * scale * rotate * model;
	drawCube(VAO, shader, model);	
	shader.setBool("exposedToSun", false);
	translate = glm::translate(identity, glm::vec3(-0.05f, 0.0f, 0.0f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	model = glm::translate(model, glm::vec3(-0.05f, 0.0f, 0.0f));

	// opposite to road side of 1st classroom
	translate = glm::translate(identity, glm::vec3(-crWidth + 0.1f, 0.0f, 0.0f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	shader.setBool("exposedToSun", true);
	translate = glm::translate(identity, glm::vec3(0.05f, 0.0f, 0.0f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	model = glm::translate(model, glm::vec3(-0.05f, 0.0f, 0.0f));

	 // farside wall of 1st classroom
	shader.setBool("exposedToSun", true);
	rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	translate = glm::translate(identity, glm::vec3(-crWidth/2.0f, 0.1f, crWidth / 2.0f));
	model = alTogether * translate * rotate * model;
	drawCube(VAO, shader, model);
	shader.setBool("exposedToSun", false);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.05f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.05f));

	// nearside wall of 1st classroom
	shader.setBool("exposedToSun", true);
	translate = glm::translate(identity, glm::vec3(-0.25f, 0.0f, crWidth-0.14));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);
	shader.setBool("exposedToSun", false);
	translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -0.05f));
	model = alTogether * translate * model;
	drawCube(VAO, shader, model);

	/*glm::mat4 offset = glm::translate(identity, glm::vec3(-crWidth+0.2f, 0.0f, 0.0f));
	for (int row = 0; row < 4; row++) {
		glm::mat4 offset = glm::translate(identity, glm::vec3(-crWidth + 0.2f, 0.0f, 0.0f));
		for (int col = 0; col < 4; col++) {
			table(VAO, shader, offset, alTogether);
		}
	}*/
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