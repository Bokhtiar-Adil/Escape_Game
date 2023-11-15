/*
	Developed by -
	Bokhtiar Adil Prottoy,
	Department of Computer Science and Engineering,
	Khulna University of Engineering & Technology,
	Khulna, Bangladesh
*/

#ifndef world_h 
#define world_h  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Shader.h"
#include "Cube.h"
#include "Sphere.h"
#include "Components.h"
#include "Curves.h"

class World
{
private:

	glm::mat4 identity, model, model2, modelTogether, scale, translate, rotate, spheremodel;
	unsigned int dMap, sMap;
	float shininess = 32.0f;
	glm::vec3 amb, diff, spec;

	Cube cube = Cube();
	Components component = Components(1200, 700);
	Sphere sphere = Sphere();
	Curves hillCurves, hillCurves2;

	float roadLength, roadWidth, garageWidth, garageLength, blockWidth, blockLength, skyLength, skyWidth, baseHeight, baseWidth;
	unsigned int textureID;
	int widthtex, heighttex, nrComponents;
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

	void loadAllCurves()
	{
		vector<float> hill1 = { 131, 391, 148, 362, 184, 341, 225, 316, 248, 267, 263, 226, 284, 185, 317, 160, 343, 110, 372, 80, 429, 54, 478, 50, 558, 88, 579, 73, 607, 61 };
		hillCurves.setControlPointsV2(hill1);

		vector<float> hill2 = {136, 394, 158, 386, 195, 364, 247, 306, 277, 256, 328, 185, 388, 142, 483, 107, 566, 98, 631, 97};
		hillCurves2.setControlPointsV2(hill2);
	}

	unsigned int grass, skytex, cloudtex;

	void loadAllTextures()
	{
		grass = loadTexture("Resources/grass.jpg");
	}

public:

	World()
	{
		identity = glm::mat4(1.0f);

		loadAllTextures();
		loadAllCurves();
	}

	

	void road(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		roadLength = 30.0f;
		roadWidth = 4.0f;

		this->amb = glm::vec3(0.1f, 0.1f, 0.1f);
		this->diff = glm::vec3(0.3f, 0.3f, 0.3f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		shader.use();
		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(roadWidth, 0.1f, roadLength));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		this->amb = glm::vec3(0.1f, 0.1f, 0.1f);
		this->diff = glm::vec3(1.0f, 1.0f, 0.0f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		model = identity;
		scale = glm::scale(identity, glm::vec3(0.1f, 0.01f, roadLength));
		translate = glm::translate(identity, glm::vec3(0.2f, 0.1f, 0.0f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(3.5f, 0.0f, 0.0f));
		model = translate * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		this->amb = glm::vec3(0.1f, 0.1f, 0.1f); 
		this->diff = glm::vec3(1.0f, 1.0f, 1.0f);
		this->spec = glm::vec3(0.0, 0.0, 0.0); 

		scale = glm::scale(identity, glm::vec3(1.0f, 1.0f, 0.03f));
		translate = glm::translate(identity, glm::vec3(-1.7f, 0.0f, 0.5f));
		model = translate * scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		for (int i = 0; i < 14; i++) {
			translate = glm::translate(identity, glm::vec3(0.0f, 0.0f, 2.0f));
			model = translate * model;
			modelTogether = alTogether * model;
			cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
		}
	}

	void garage(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		garageWidth = 4.0f;
		garageLength = 5.0f;

		this->amb = glm::vec3(0.1f, 0.1f, 0.1f);
		this->diff = glm::vec3(0.7f, 0.7f, 0.7f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(garageWidth, 0.1f, garageLength));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
	}

	void residential(Shader& shader, bool withTexture, bool left, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		blockWidth = 3.0f;
		blockLength = 15.0f;

		this->dMap = grass;
		this->sMap = grass;

		this->amb = glm::vec3(0.0f, 0.1f, 0.0f);
		this->diff = glm::vec3(0.0f, 0.7f, 0.0f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		shader.setBool("exposedToSun", true);
		model = identity;
		scale = glm::scale(identity, glm::vec3(blockWidth, 0.1f, blockLength));
		model = scale * model;
		modelTogether = alTogether * model;
		if (withTexture) cube.drawCubeWithTexture(shader, dMap, sMap, shininess, modelTogether);
		else cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		if (left) {
			rotate = glm::rotate(identity, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			for (int i = 0; i < 15; i++) {
				component.wall(shader, true, alTogether * glm::translate(identity, glm::vec3(-0.002f * blockWidth, 0.0f, 7.0f - 1.0f*i)) * rotate);
			}
		}
		else {
			rotate = glm::rotate(identity, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			for (int i = 0; i < 20; i++) {
				component.wall(shader, true, alTogether * glm::translate(identity, glm::vec3(blockWidth - 0.05f, 0.0f, 7.0f - 1.0f * i)) * rotate);
			}
		}
		
	}

	void hills(Shader& shaderMP, Shader& shaderCurves, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseHeight = 4.0f;
		blockWidth = 3.0f;
		blockLength = 15.0f;
		
		this->amb = glm::vec3(0.45f, 0.347f, 0.265f);
		this->diff = glm::vec3(0.45f, 0.347f, 0.265f);
		this->spec = glm::vec3(0.0f, 0.0f, 0.0f);

		shaderCurves.use();
		shaderCurves.setBool("exposedToSun", true);
		shaderCurves.setBool("overrideColor", true);
		shaderCurves.setVec3("color", glm::vec3(0.035f, 0.52f, 0.28f));
		model = identity;
		scale = glm::scale(identity, glm::vec3(blockWidth, baseHeight, blockLength * 0.3f));		
		model = scale;
		modelTogether = alTogether * model;
		hillCurves.setModel(modelTogether);
		hillCurves.drawCurves(shaderCurves, this->amb, this->diff, this->spec);
		model2 = model;

		model = glm::translate(identity, glm::vec3(0.0f, 0.0f, 2.0f)) * glm::scale(identity, glm::vec3(1.0f, 0.75f, 1.0f)) * model;
		modelTogether = alTogether * model;
		hillCurves.setModel(modelTogether);
		hillCurves.drawCurves(shaderCurves, this->amb, this->diff, this->spec);

		model = glm::translate(identity, glm::vec3(0.0f, 0.0f, -2.0f)) * glm::scale(identity, glm::vec3(1.0f, 0.45f, 1.0f)) * model2;
		modelTogether = alTogether * model;
		hillCurves.setModel(modelTogether);
		hillCurves.drawCurves(shaderCurves, this->amb, this->diff, this->spec);
	}

	void hillSinglePeak(Shader& shaderMP, Shader& shaderCurves, bool withTexture, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		baseHeight = 4.0f;
		blockWidth = 3.0f;
		blockLength = 15.0f;

		this->amb = glm::vec3(0.45f, 0.347f, 0.265f);
		this->diff = glm::vec3(0.45f, 0.347f, 0.265f);
		this->spec = glm::vec3(0.0f, 0.0f, 0.0f);

		shaderCurves.use();
		shaderCurves.setBool("exposedToSun", true);
		shaderCurves.setBool("overrideColor", true);
		shaderCurves.setVec3("color", glm::vec3(0.035f, 0.52f, 0.28f));
		model = identity;
		scale = glm::scale(identity, glm::vec3(blockWidth, baseHeight, blockLength));		
		model = scale;
		modelTogether = alTogether * model;
		hillCurves.setModel(modelTogether);
		hillCurves.drawCurves(shaderCurves, this->amb, this->diff, this->spec);
	}
};

#endif /* world_h */