#ifndef collectorItems_h 
#define collectorItems_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Cube.h"
#include "Shader.h"
#include "Sphere.h"
#include "Curves.h"

class CollectorItems
{
private:

	int win_width, win_height;

	glm::mat4 identity, model, modelTogether, scale, translate, rotate, spheremodel;
	unsigned int dMap, sMap;
	float shininess = 32.0f;
	glm::vec3 amb, diff, spec;

	Cube cube = Cube();
	Sphere sphere = Sphere();
	Curves treeCurves = Curves();

	unsigned int cylinderVAO;
	unsigned int cylinderVBO;
	unsigned int cylinderEBO;

	float xVal, yVal, baseWidth, baseWidthz, baseHeight, legWidth, height, widthx, widthz, boxWidth, boxHeight, baseLength, theta;
	int widthtex, heighttex, nrComponents;
	unsigned int textureID;
	unsigned char* data;
	GLenum format;

	unsigned int loadTexture(char const* path)
	{
		glGenTextures(1, &textureID);

		data = stbi_load(path, &widthtex, &heighttex, &nrComponents, 0);
		if (data)
		{

			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, widthtex, heighttex, 0, format, GL_UNSIGNED_BYTE, data);
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

	unsigned int loadTextureV2(char const* path)
	{
		glGenTextures(1, &textureID);

		data = stbi_load(path, &widthtex, &heighttex, &nrComponents, 0);
		if (data)
		{

			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, widthtex, heighttex, 0, format, GL_UNSIGNED_BYTE, data);
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

	void setupCylinder(float theta = 45.0f)
	{
		yVal = glm::sin(glm::radians(theta));
		xVal = glm::cos(glm::radians(theta));

		float cylinder_vertices[] = {
			// positions      // normals
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			xVal, yVal, 0.0f, 0.0f, 0.0f, 1.0f,

			0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
			xVal, yVal, -1.0f, 0.0f, 0.0f, -1.0f,

			0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,

			1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			xVal, yVal, 0.0f, 1.0f, 0.0f, 0.0f,
			xVal, yVal, -1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,

		};
		unsigned int cylinder_indices[] = {
			0,1,2,

			3,4,5,

			6,7,8,
			6,8,9,

			10,11,12,
			10,12,13,
		};

		glGenVertexArrays(1, &cylinderVAO);
		glGenBuffers(1, &cylinderVBO);
		glGenBuffers(1, &cylinderEBO);


		glBindVertexArray(cylinderVAO);

		glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder_vertices), cylinder_vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinder_indices), cylinder_indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// vertex normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
		glEnableVertexAttribArray(1);


	}

	void drawCylinder(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f), float theta = 45.0f)
	{
		shader.use();

		/*this->amb = glm::vec3(1.0f, 0.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 0.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);*/

		shader.setVec3("material.ambient", this->amb);
		shader.setVec3("material.diffuse", this->diff);
		shader.setVec3("material.specular", this->spec);
		shader.setFloat("material.shininess", this->shininess);
		shader.setBool("exposedToSun", true);

		rotate = glm::rotate(identity, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::mat4(1.0f);
		int portions = (int)(360 / theta);
		for (int i = 0; i <= portions; i++) {
			modelTogether = alTogether * model;
			shader.setMat4("model", modelTogether);

			glBindVertexArray(cylinderVAO);
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

			model = rotate * model;
		}
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &cylinderVAO);
		glDeleteBuffers(1, &cylinderVBO);
		glDeleteBuffers(1, &cylinderEBO);

	}

	// unsigned int boxtex, walldmp, wallsmp, tabledmp, doordmp, walldmp2, windowsdmp, grass, grass2, palestine1, noexit, seventyone, mosqueWall, door2;

	void loadAllTextures()
	{
		//boxtex = loadTexture("container2.png");
		//walldmp = loadTexture("brickwall.jpg");
		//wallsmp = loadTexture("brickwall.jpg");
		//tabledmp = loadTexture("woodenSurface.jpg");
		//doordmp = loadTexture("woodenDoor.jpg");
		//walldmp2 = loadTexture("whitewall2.jpg");
		//windowsdmp = loadTexture("windows.jpg");
		//grass = loadTexture("grass.jpg");
		//grass2 = loadTextureV2("grass.jpg");
		////palestine1 = loadTexture("palestine.jpg");
		//noexit = loadTexture("no_exit.jpg");
		//seventyone = loadTexture("seventyone1.jpg");
		//mosqueWall = loadTexture("mosque_wall.jpg");
		//door2 = loadTexture("door2.jpg");
	}

	void loadAllCurves()
	{
		/*vector<float> tree3 = { 611, 48, 572, 46, 507, 56, 435, 89, 407, 133, 380, 207, 394, 260, 375, 297, 319, 357, 299, 440, 364, 511, 437, 554, 502, 568, 549, 559, 579, 519 };
		treeCurves.setControlPointsV2(tree3);*/
	}

public:

	CollectorItems()
	{
		identity = glm::mat4(1.0f);

		loadAllTextures();
		loadAllCurves();
		treeCurves.setWinProperties(this->win_width, this->win_height);

	}

	void boostBonusItem(Shader& shader1, Shader& shader2, int whichShader = 1, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		boxWidth = 0.2f;

		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.f);
		this->spec = glm::vec3(1.0f, 1.0f, 1.0f);

		if (whichShader == 1) {
			shader1.use();
			shader1.setBool("exposedToSun", true);

			scale = glm::scale(identity, glm::vec3(boxWidth * 2.0f, boxWidth, boxWidth * 0.5f));
			//rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(-0.1f, 0.1f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader1, this->amb, this->diff, this->spec, this->shininess, modelTogether);

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader1, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}
		else {
			shader2.use();
			//shader2.setBool("exposedToSun", true);
			shader2.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));

			scale = glm::scale(identity, glm::vec3(boxWidth * 2.0f, boxWidth, boxWidth * 0.5f));
			//rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(-0.1f, 0.1f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader2, this->amb, this->diff, this->spec, this->shininess, modelTogether);

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader2, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}
		
	}

	void coinBonusItem(Shader& shader1, Shader& shader2, int whichShader = 1, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseWidth = 0.2f;
		
		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.f);
		this->spec = glm::vec3(1.0f, 1.0f, 1.0f);

		theta = 10.0f;
		scale = glm::scale(identity, glm::vec3(baseWidth, baseWidth, baseWidth*0.5f));
		translate = glm::translate(identity, glm::vec3(0.0f, 0.1f, 0.0f));
		model = translate * scale * identity;
		modelTogether = alTogether * model;
		setupCylinder(theta);

		if (whichShader == 1) {	
			shader1.use();
			shader1.setBool("exposedToSun", true);
			drawCylinder(shader1, modelTogether, theta);
		}
		else {
			shader2.use();
			shader2.setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f));
			drawCylinder(shader2, modelTogether, theta);
		}		
		
	}

	void fuelBonusItem(Shader& shader1, Shader& shader2, int whichShader = 1, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		boxWidth = 0.2f;

		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.f);
		this->spec = glm::vec3(1.0f, 1.0f, 1.0f);

		if (whichShader == 1) {
			shader1.use();
			shader1.setBool("exposedToSun", true);			

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader1, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}
		else {
			shader2.use();
			//shader2.setBool("exposedToSun", true);
			shader2.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));			

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader2, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}

	}

	void darkBonusItem(Shader& shader1, Shader& shader2, int whichShader = 1, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		boxWidth = 0.2f;

		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.f);
		this->spec = glm::vec3(1.0f, 1.0f, 1.0f);

		if (whichShader == 1) {
			shader1.use();
			shader1.setBool("exposedToSun", true);

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader1, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}
		else {
			shader2.use();
			//shader2.setBool("exposedToSun", true);
			shader2.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth, boxWidth));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader2, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}

	}

	void slowBonusItem(Shader& shader1, Shader& shader2, int whichShader = 1, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		boxWidth = 0.2f;

		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.f);
		this->spec = glm::vec3(1.0f, 1.0f, 1.0f);

		rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		if (whichShader == 1) {
			shader1.use();
			shader1.setBool("exposedToSun", true);

			scale = glm::scale(identity, glm::vec3(boxWidth * 2.0f, boxWidth, boxWidth * 0.5f));
			//rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(-0.1f, 0.1f, 0.0f));
			modelTogether = alTogether * rotate * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader1, this->amb, this->diff, this->spec, this->shininess, modelTogether);

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * rotate * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader1, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}
		else {
			shader2.use();
			//shader2.setBool("exposedToSun", true);
			shader2.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));

			scale = glm::scale(identity, glm::vec3(boxWidth * 2.0f, boxWidth, boxWidth * 0.5f));
			//rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(-0.1f, 0.1f, 0.0f));
			modelTogether = alTogether * rotate * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader2, this->amb, this->diff, this->spec, this->shininess, modelTogether);

			scale = glm::scale(identity, glm::vec3(boxWidth, boxWidth * 2.0f, boxWidth * 0.5f));
			// rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
			modelTogether = alTogether * rotate * translate * scale;
			cube.drawCubeWithMaterialisticProperty(shader2, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}

	}

};

#endif /* collectorItems_h */
