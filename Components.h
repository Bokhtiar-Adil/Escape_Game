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

public:

	Components()
	{
		identity = glm::mat4(1.0f);
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

	void table(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		float baseWidth = 0.6f;
		float baseWidthz = 0.4f;
		float baseHeight = 0.05f;
		float legWidth = 0.1f;

		if (withTexture) {
			this->dMap = loadTexture("woodenSurface.jpg");
			this->sMap = loadTexture("woodenSurface.jpg");
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
		float baseWidth = 0.25f;
		float baseHeight = 0.05f;
		float legWidth = 0.05f;
		
		if (withTexture) {
			this->dMap = loadTexture("woodenSurface.jpg");
			this->sMap = loadTexture("woodenSurface.jpg");
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

	void building_notex(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		float height = 2.0f;
		float widthx = 3.0f;
		float widthz = 2.0f;

		if (withTexture) {
			this->dMap = loadTexture("whitewall2.jpg");
			this->sMap = loadTexture("whitewall2.jpg");
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
			this->dMap = loadTexture("woodenDoor.jpg");
			this->sMap = loadTexture("woodenDoor.jpg");
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
		float height = 2.0f;
		float widthx = 3.0f;
		float widthz = 2.0f;

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
		this->dMap = loadTexture("woodenDoor.jpg");
		this->sMap = loadTexture("woodenDoor.jpg");
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
		this->dMap = loadTexture("woodenDoor.jpg");
		this->sMap = loadTexture("woodenDoor.jpg");

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
		float baseWidth = 1.0f;
		float baseWidthz = 0.4f;
		float baseHeight = 0.05f;
		float legWidth = 0.1f;

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

	void car(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{

	}

	void box(Shader& shader, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		float boxWidth = 0.5f;
		float boxHeight = 0.5f; 
	
		if (withTexture) {
			this->dMap = loadTexture("container2.png");
			this->sMap = loadTexture("container2_specular.png");
		}
		else {
			this->amb = glm::vec3(0.88f, 0.88f, 0.88f);
			this->diff = glm::vec3(0.88f, 0.88f, 0.88f);
			this->spec = glm::vec3(0.3, 0.3, 0.3);
		}

		shader.setBool("exposedToSun", true);
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


};



#endif /* components_h */
