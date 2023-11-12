#ifndef world_h 
#define world_h  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Shader.h"
#include "Cube.h"
#include "Components.h"

class World
{
private:

	glm::mat4 identity, model, modelTogether, scale, translate, rotate;
	unsigned int dMap, sMap;
	float shininess = 32.0f;
	glm::vec3 amb, diff, spec;

	Cube cube = Cube();
	Components component = Components(1200, 700);

	float roadLength, roadWidth, garageWidth, garageLength, blockWidth, blockLength, skyLength, skyWidth;
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

	unsigned int grass, skytex;

	void loadAllTextures()
	{
		grass = loadTexture("grass.jpg");
		skytex= loadTexture("sky1.jpg");
	}

public:

	World()
	{
		identity = glm::mat4(1.0f);

		loadAllTextures();
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

		/*translate = glm::translate(identity, glm::vec3(4.0f, 0.0f, 10.0f));
		modelTogether = alTogether * translate * identity;
		component.tree(shader, shaderCurves, false, modelTogether);*/
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

	void sky(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f))
	{
		skyWidth = 50.0f;
		skyLength = 40.0f;
		
		this->dMap = skytex;

		model = identity;
		scale = glm::scale(identity, glm::vec3(skyWidth, 0.1f, skyLength));
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithTexture(shader, dMap, dMap, shininess, modelTogether);

		model = identity;
		scale = glm::scale(identity, glm::vec3(skyWidth, 10.0f, 0.1));	
		
		model = scale * model;
		modelTogether = alTogether * model;
		cube.drawCubeWithTexture(shader, dMap, dMap, shininess, modelTogether);

	}
};

#endif /* world_h */