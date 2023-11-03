#ifndef components_h
#define components_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Cube.h"
#include "Shader.h"

class Components
{
private:

	glm::mat4 identity, model, modelTogether, scale, translate, rotate;
	unsigned int dMap, sMap;
	float shininess = 32.0f;
	glm::vec3 amb, diff, spec;

	Cube cube = Cube();

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

		this->amb = glm::vec3(1.0f, 0.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 0.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		shader.setVec3("material.ambient", this->amb);
		shader.setVec3("material.diffuse", this->diff);
		shader.setVec3("material.specular", this->spec);
		shader.setFloat("material.shininess", this->shininess);
		shader.setBool("exposedToSun", true);

		rotate = glm::rotate(identity, glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::mat4(1.0f);
		int portions = (int)(360 / theta);
		for (int i = 0; i < portions; i++) {		
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

	unsigned int boxtex, walldmp, wallsmp, tabledmp, doordmp, walldmp2, windowsdmp;

	void loadAllTextures()
	{
		boxtex = loadTexture("container2.png");
		walldmp = loadTexture("brickwall.jpg");
		wallsmp = loadTexture("brickwall.jpg");
		tabledmp = loadTexture("woodenSurface.jpg");
		doordmp = loadTexture("woodenDoor.jpg");
		walldmp2 = loadTexture("whitewall2.jpg");
		windowsdmp = loadTexture("windows.jpg");
	}

public:

	Components()
	{
		identity = glm::mat4(1.0f);

		loadAllTextures();
	}	

	void table(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseWidth = 0.6f;
		baseWidthz = 0.4f;
		baseHeight = 0.05f;
		legWidth = 0.1f;

		if (withTexture) {
			this->dMap = tabledmp;
			this->sMap = tabledmp;
		}
		else {
			this->amb = glm::vec3(1.0f, 0.0f, 0.0f);
			this->diff = glm::vec3(1.0f, 0.0f, 0.0f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}

		// base
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseWidth, baseHeight, baseWidthz));		
		model = scale * model;
		modelTogether = alTogether * model; 
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// legs

		this->amb = glm::vec3(1.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		// back-left
		scale = glm::scale(identity, glm::vec3(0.1, 5.0f, 0.1));
		translate = glm::translate(identity, glm::vec3(0.0f, -5.0f * baseHeight, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// front-left
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, baseWidthz - legWidth * 0.4f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// front-right
		translate = glm::translate(identity, glm::vec3(baseWidth-legWidth*0.6f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// back-right
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1.0f * baseWidthz + legWidth * 0.4f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

	}

	void chair(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseWidth = 0.25f;
		baseHeight = 0.05f;
		legWidth = 0.05f;
		
		if (withTexture) {
			this->dMap = tabledmp;
			this->sMap = tabledmp;
		}
		else {
			this->amb = glm::vec3(0.0f, 1.0f, 1.0f);
			this->diff = glm::vec3(0.0f, 1.0f, 1.0f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}

		// backrest
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseWidth, legWidth*0.5f, baseWidth*0.9f));
		rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		translate = glm::translate(identity, glm::vec3(0.0f, -0.1f, 0.025f));		
		model = translate * rotate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// base
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseWidth, baseHeight, baseWidth));
		translate = glm::translate(identity, glm::vec3(0.0f, -0.1f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// legs 

		this->amb = glm::vec3(1.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		// back-left
		scale = glm::scale(identity, glm::vec3(0.1, 3.5f, 0.1));
		translate = glm::translate(identity, glm::vec3(0.0f, 0.1f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// front-left
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, baseWidth - legWidth * 0.6f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// front-right
		translate = glm::translate(identity, glm::vec3(baseWidth - legWidth * 0.6f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// back-right
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1.0f * baseWidth + legWidth * 0.6f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
	}

	void building(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		height = 2.0f;
		widthx = 3.0f;
		widthz = 2.0f;

		if (withTexture) {
			this->dMap = walldmp2;
			this->sMap = walldmp2;
		}
		else {
			this->amb = glm::vec3(0.88f, 0.88f, 0.88f);
			this->diff = glm::vec3(0.88f, 0.88f, 0.88f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}

		shader.setBool("exposedToSun", true);

		// outer shape		
		model = identity;
		scale = glm::scale(identity, glm::vec3(widthx, height, widthz));
		model = scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// door
		if (withTexture) {
			this->dMap = doordmp;
			this->sMap = doordmp;
		}
		else {
			this->amb = glm::vec3(0.43f, 0.33f, 0.26f);
			this->diff = glm::vec3(0.43f, 0.33f, 0.26f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.5f, 0.8f, 0.02f));
		translate = glm::translate(identity, glm::vec3(1.3f, 0.0f, 1.99f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		glm::mat4 model2 = model;
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.01f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		if (withTexture) {
			this->dMap = windowsdmp;
			this->sMap = windowsdmp;
		}
		else {
			this->amb = glm::vec3(0.43f, 0.33f, 0.26f);
			this->diff = glm::vec3(0.43f, 0.33f, 0.26f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}
		
		// front side wall windows
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.5f, 0.5f, 0.02f));
		translate = glm::translate(identity, glm::vec3(0.5f, 0.3f, 1.99f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		model2 = model;

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(1.6f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// back side wall windows
		model = model2;		
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.01f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);		

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(1.6f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// nearside wall windows
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.02f, 0.5f, 0.5f));
		translate = glm::translate(identity, glm::vec3(3.0f, 0.3f, 0.2f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		model2 = model;

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 1.1f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// farside wall windows		
		translate = glm::translate(identity, glm::vec3(-3.01f, 0.0f, 0.0f));
		model = translate * model2;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 1.1f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

	}

	void building_texdoor(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		height = 2.0f;
		widthx = 3.0f;
		widthz = 2.0f;

		shader.setBool("withTexture", false);
		this->amb = glm::vec3(0.88f, 0.88f, 0.88f);
		this->diff = glm::vec3(0.88f, 0.88f, 0.88f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		shader.setBool("exposedToSun", true);

		// outer shape		
		model = identity;
		scale = glm::scale(identity, glm::vec3(widthx, height, widthz));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// door
		shader.setBool("withTexture", true);
		this->dMap = doordmp;
		this->sMap = doordmp;
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.5f, 0.8f, 0.02f));
		translate = glm::translate(identity, glm::vec3(1.3f, 0.0f, 1.99f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithTextureV2(shader, dMap, sMap, this->shininess, modelTogether);
		glm::mat4 model2 = model;
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.01f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithTextureV2(shader, dMap, sMap, this->shininess, modelTogether);

		shader.setBool("withTexture", false);
		this->amb = glm::vec3(0.43f, 0.33f, 0.26f);
		this->diff = glm::vec3(0.43f, 0.33f, 0.26f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		// front side wall windows
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.5f, 0.5f, 0.02f));
		translate = glm::translate(identity, glm::vec3(0.5f, 0.3f, 1.99f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		model2 = model;

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(1.6f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// back side wall windows
		model = model2;
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.01f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(1.6f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// nearside wall windows
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.02f, 0.5f, 0.5f));
		translate = glm::translate(identity, glm::vec3(3.0f, 0.3f, 0.2f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		model2 = model;

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 1.1f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// farside wall windows		
		translate = glm::translate(identity, glm::vec3(-3.01f, 0.0f, 0.0f));
		model = translate * model2;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 1.1f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, -0.9f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

	}

	void door_tex(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		this->dMap = doordmp;
		this->sMap = doordmp;

		// base
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(0.5f, 1.0f, 0.02f));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
	}

	void bench(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseWidth = 1.0f;
		baseWidthz = 0.4f;
		baseHeight = 0.05f;
		legWidth = 0.1f;

		this->amb = glm::vec3(1.0f, 0.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 0.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		// base
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseWidth, baseHeight, baseWidthz));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		
		// backrest

		this->amb = glm::vec3(1.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		rotate = glm::rotate(identity, glm::radians(-105.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.01f));
		model = translate  * rotate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// leg

		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 1.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		rotate = glm::rotate(identity, glm::radians(195.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		scale = glm::scale(identity, glm::vec3(1.0f, 0.6f, 1.5f));
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -0.03f));
		model = translate * scale * rotate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// armrests

		this->amb = glm::vec3(1.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		scale = glm::scale(identity, glm::vec3(0.43f, 0.45f, 0.8f));
		rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identity, glm::vec3(0.035f, 0.14f, 0.4f));
		model = translate * rotate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.94f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
	}

	void box(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		boxWidth = 0.5f;
		boxHeight = 0.5f;

		if (withTexture) {
			this->dMap = boxtex;
			//this->sMap = loadTexture("container2_specular.png");
		}
		else {
			this->amb = glm::vec3(0.88f, 0.88f, 0.88f);
			this->diff = glm::vec3(0.88f, 0.88f, 0.88f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}

		shader.setBool("exposedToSun", true);

		scale = glm::scale(identity, glm::vec3(boxWidth, boxHeight, boxWidth));
		//translate = glm::translate(identity, glm::vec3(0.5f, 0.5f, 0.0f));
		modelTogether = alTogether * scale;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, dMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
	}

	void car(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseWidth = 1.2f; 
		baseLength = 2.0f; 
		baseHeight = 0.2f; 
		theta = 22.5f;

		shader.setBool("exposedToSun", true);

		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		// base
		
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseLength, baseHeight, baseWidth));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);		

		// sides

		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 1.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		scale = glm::scale(identity, glm::vec3(0.05f, 1.2f, 1.0f));
		translate = glm::translate(identity, glm::vec3(0.5f, 0.20f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(1.4f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identity, glm::vec3(0.5f, -0.01f, 2.0f));
		scale = glm::scale(identity, glm::vec3(1.2f, 1.0f, 1.0f));
		model = translate * scale * rotate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 1.1f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identity, glm::vec3(1.5f, 0.0f, -0.6f));
		model = translate * rotate * model;
		rotate = glm::rotate(identity, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = rotate * model;
		scale = glm::scale(identity, glm::vec3(1.6f, 1.0f, 0.8f));
		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.1f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);


		// seats
		scale = glm::scale(identity, glm::vec3(0.4f, 0.6f, 0.5f));
		rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identity, glm::vec3(1.1f, 0.3f, 0.6f));
		glm::mat4 seats = translate * rotate * scale;
		glm::mat4 seatsTogether = alTogether * seats;
		bench(shader, seatsTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -0.4f));
		seats = translate * seats;
		seatsTogether = alTogether * seats;
		bench(shader, seatsTogether);

		translate = glm::translate(identity, glm::vec3(0.5f, 0.0f, 0.0f));
		seats = translate * seats;
		seatsTogether = alTogether * seats;
		bench(shader, seatsTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.4f));
		seats = translate * seats;
		seatsTogether = alTogether * seats;
		bench(shader, seatsTogether);

		// tires
		glm::mat4 tires = glm::scale(identity, glm::vec3(0.3f, 0.3f, 0.2f));
		translate = glm::translate(identity, glm::vec3(0.5f, 0.0f, 1.4f));
		tires = translate * tires;
		glm::mat4 tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

		translate = glm::translate(identity, glm::vec3(1.4f, 0.0f, 0.0f));
		tires = translate * tires;
		tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1.4f));
		tires = translate * tires;
		tiresTogether = alTogether * tires; 
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

		translate = glm::translate(identity, glm::vec3(-1.4f, 0.0f, 0.0f));
		tires = translate * tires;
		tiresTogether = alTogether * tires; 
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);
		
		// steering wheel
		scale = glm::scale(identity, glm::vec3(0.05f, 0.05f, 0.01f));
		translate = glm::translate(identity, glm::vec3(0.7511f, 0.35f, 0.4f));
		rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotate2 = glm::rotate(identity, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		tires = translate * rotate2 * rotate * scale;
		tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);
	}

	void truck(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseWidth = 1.2f;
		baseLength = 2.5f;
		baseHeight = 0.3f;
		theta = 22.5f;

		shader.setBool("exposedToSun", true);

		this->amb = glm::vec3(0.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		// base
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseLength-0.8f, baseHeight, baseWidth));
		translate = glm::translate(identity, glm::vec3(0.8f, 0.0f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// head

		this->amb = glm::vec3(1.0f, 1.0f, 0.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		model = identity;
		scale = glm::scale(identity, glm::vec3(0.8f, 1.0f, baseWidth));
		//translate = glm::translate(identity, glm::vec3(0.0f, 0.3f, 0.0f));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// cargoarea

		this->amb = glm::vec3(0.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(0.0f, 1.0f, 1.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		model = identity;
		scale = glm::scale(identity, glm::vec3(baseLength - 0.8f, 0.7f, 0.01f));
		translate = glm::translate(identity, glm::vec3(0.8f, 0.3f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, baseWidth));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		model = identity;
		scale = glm::scale(identity, glm::vec3(0.01f, 0.7f, baseWidth));
		translate = glm::translate(identity, glm::vec3(0.8f, 0.3f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(baseLength-0.8f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// head glass

		this->amb = glm::vec3(1.0f, 0.0f, 1.0f);
		this->diff = glm::vec3(1.0f, 0.0f, 1.0f);
		this->spec = glm::vec3(0.3, 0.3, 0.3);

		model = identity;
		scale = glm::scale(identity, glm::vec3(0.01f, 0.5f, baseWidth)); 
		translate = glm::translate(identity, glm::vec3(-0.01f, 0.5f, 0.0f));
		model = translate * scale;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		model = identity;
		scale = glm::scale(identity, glm::vec3(0.8f, 0.5f, 0.01f));
		translate = glm::translate(identity, glm::vec3(0.0f, 0.5f, -0.01f));
		model = translate * scale;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, baseWidth+0.02f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// tires
		glm::mat4 tires = glm::scale(identity, glm::vec3(0.3f, 0.3f, 0.2f));
		translate = glm::translate(identity, glm::vec3(0.5f, 0.0f, 1.4f));
		tires = translate * tires;
		glm::mat4 tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

		translate = glm::translate(identity, glm::vec3(1.4f, 0.0f, 0.0f));
		tires = translate * tires;
		tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

		translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, -1.4f));
		tires = translate * tires;
		tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

		translate = glm::translate(identity, glm::vec3(-1.4f, 0.0f, 0.0f));
		tires = translate * tires;
		tiresTogether = alTogether * tires;
		setupCylinder(theta);
		drawCylinder(shader, tiresTogether, theta);

	}

	void wall(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		if (withTexture) {
			this->dMap = walldmp;
			this->sMap = wallsmp;
		}
		else {
			this->amb = glm::vec3(0.1f, 0.1f, 0.1f);
			this->diff = glm::vec3(1.0f, 1.0f, 1.0f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}
		

		// base
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(1.0f, 1.0f, 0.02f));
		model = scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, this->shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
	}

	void streetlight(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseHeight = 2.0f;
		baseWidth = 0.05f;

		this->amb = glm::vec3(0.05f, 0.05f, 0.05f);
		this->diff = glm::vec3(0.7f, 0.7f, 0.7f);
		this->spec = glm::vec3(0.5, 0.5, 0.5);
		
		// base
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseWidth, baseHeight, baseWidth));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = identity;
		scale = glm::scale(identity, glm::vec3(baseWidth, baseHeight*0.1f, baseWidth));
		translate = glm::translate(identity, glm::vec3(0.0f, 2.0f, 0.0f));
		model = translate * rotate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 1.0f);
		this->spec = glm::vec3(1.0f, 1.0f, 1.0f);

		scale = glm::scale(identity, glm::vec3(0.5f, 1.0f, 1.0f));
		translate = glm::translate(identity, glm::vec3(0.1f, -0.05f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		
		
	}

	

};



#endif /* components_h */
