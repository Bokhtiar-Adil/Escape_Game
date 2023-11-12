#ifndef character_h 
#define character_h  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Shader.h"
#include "Cube.h"
#include "Sphere.h"
#include "Curves.h"


class Character
{
private:

	glm::mat4 identity, model, modelTogether, scale, translate, translate2, rotate, spheremodel, cubemodel, cubemodel2, spheremodel2;
	unsigned int dMap, sMap;
	float shininess = 32.0f;
	glm::vec3 amb, diff, spec;

	float roadLength, roadWidth, garageWidth, garageLength, blockWidth, blockLength;
	unsigned int textureID;
	int widthtex, heighttex, nrComponents;
	unsigned char* data;
	GLenum format;

	Cube cube = Cube();
	Sphere sphere = Sphere();
	Curves hat;

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

	void loadAllTextures()
	{
		//grass = loadTexture("grass.jpg");
	}

	/*void loadAllCurves()
	{
		vector<float> hatpoints = { 642, 78, 569, 58, 495, 55, 459, 107, 416, 148, 340, 164, 289, 238, 192, 247, 119, 354, 45, 434 };
		hat.setControlPoints(hatpoints);
	}*/

public:

	Character()
	{
		identity = glm::mat4(1.0f);

		loadAllTextures();
	}

	void drawProtagonist(Shader& shader, glm::mat4 alTogether = glm::mat4(1.0f), string state = "still")
	{
		shader.use();

		// head
		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(1.0f, 1.0f, 1.0f);
		this->spec = glm::vec3(0.7f, 0.7f, 0.7f);
		sphere.setMaterialisticProperties(this->amb, this->diff, this->spec);

		spheremodel = identity;
		scale = glm::scale(identity, glm::vec3(0.15f, 0.15f, 0.15f));
		translate = glm::translate(identity, glm::vec3(0.0f, -0.07f, 0.0f));
		spheremodel = translate * scale * identity;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		// torso-head joint
		this->amb = glm::vec3(0.07f, 0.11f, 0.56f);
		this->diff = glm::vec3(0.07f, 0.11f, 0.56f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		scale = glm::scale(identity, glm::vec3(0.1f, 0.1f, 0.08f));
		translate = glm::translate(identity, glm::vec3(-0.05f, -0.3f, -0.04f));
		cubemodel = translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// torso
		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(0.8f, 0.8f, 0.8f);
		this->spec = glm::vec3(0.7f, 0.7f, 0.7f);

		scale = glm::scale(identity, glm::vec3(0.3f, 0.5f, 0.1f));
		translate = glm::translate(identity, glm::vec3(-0.15f, -0.75f, -0.05f));
		cubemodel = translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// torso-hand joint
		this->amb = glm::vec3(0.07f, 0.11f, 0.56f);
		this->diff = glm::vec3(0.07f, 0.11f, 0.56f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);
		sphere.setMaterialisticProperties(this->amb, this->diff, this->spec);

		spheremodel = identity;
		scale = glm::scale(identity, glm::vec3(0.08f, 0.08f, 0.08f));
		translate = glm::translate(identity, glm::vec3(0.23f, -0.25f, 0.0f));
		spheremodel = translate * scale * identity;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		translate = glm::translate(identity, glm::vec3(-0.455f, 0.0f, 0.0f));
		spheremodel = translate * spheremodel;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		/*spheremodel2 = spheremodel;

		translate = glm::translate(identity, glm::vec3(0.0f, -0.25f, 0.0f));
		spheremodel = translate * spheremodel;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.455f, 0.0f, 0.0f));
		spheremodel = translate * spheremodel;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		spheremodel = spheremodel2;*/

		//  torso-leg joint
		translate = glm::translate(identity, glm::vec3(0.14f, -0.57f, 0.0f));
		spheremodel = translate * spheremodel;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.17f, 0.0f, 0.0f));
		spheremodel = translate * spheremodel;
		modelTogether = alTogether * spheremodel;
		sphere.drawSphere(shader, modelTogether);

		// hands
		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(0.8f, 0.8f, 0.8f);
		this->spec = glm::vec3(0.7, 0.7, 0.7);

		//state = "right";
		scale = glm::scale(identity, glm::vec3(0.1f, 0.25f, 0.1f));
		translate = glm::translate(identity, glm::vec3(0.18f, -0.5f, -0.05f));
		cubemodel2 = translate * scale * identity;
		if (state == "right") {
			rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.08f, 0.2f));
		}
		else if (state == "left") {
			rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(-0.46f, -0.08f, 0.2f));
		}
		else {
			rotate = glm::rotate(identity, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
		}
		cubemodel = translate2 * rotate * translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		cubemodel = cubemodel2;		

		if (state == "right") {
			rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.08f, -0.2f));
		}
		else if (state == "left") {
			rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.46f, -0.08f, -0.2f));
		}
		else {
			rotate = glm::rotate(identity, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.0f, 0.0f));
		}
		
		translate = glm::translate(identity, glm::vec3(-0.46f, 0.0f, 0.0f));
		cubemodel = translate2 * rotate * translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// lower hand parts		

		if (state == "right") {
			rotate = glm::rotate(identity, glm::radians(120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.60f, 0.7f));
		}
		else if (state == "left") {
			rotate = glm::rotate(identity, glm::radians(120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.60f, 0.7f));
		}
		else {
			rotate = glm::rotate(identity, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
		}
		translate = glm::translate(identity, glm::vec3(0.0f, -0.2f, 0.0f));
		cubemodel2 = translate * cubemodel;
		cubemodel = translate2 * rotate * translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		cubemodel = cubemodel2;

		if (state == "right") {
			rotate = glm::rotate(identity, glm::radians(-120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.85f, -0.6f));
		}
		else if (state == "left") {
			rotate = glm::rotate(identity, glm::radians(-120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(-0.92f, -0.85f, -0.6f));

		}
		else {
			rotate = glm::rotate(identity, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
		}

		translate = glm::translate(identity, glm::vec3(0.46f, 0.0f, 0.0f));
		cubemodel = translate2 * rotate * translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// legs
		this->amb = glm::vec3(1.0f, 1.0f, 1.0f);
		this->diff = glm::vec3(0.8f, 0.8f, 0.8f);
		this->spec = glm::vec3(0.7f, 0.7f, 0.7f);

		//state = "right";
		if (state == "right") {
			rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.5f, -0.35f));
		}
		else if (state == "left") {
			rotate = glm::rotate(identity, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(-0.17f, -0.5f, -0.35f));
		}
		else {
			rotate = glm::rotate(identity, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.3f, 0.0f));
		}

		scale = glm::scale(identity, glm::vec3(0.1f, 0.6f, 0.1f));
		translate = glm::translate(identity, glm::vec3(0.04f, -1.1f, -0.05f));
		cubemodel2 = translate * scale * identity;
		cubemodel = translate2 * rotate * translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		cubemodel = cubemodel2;
		
		//state = "still";
		if (state == "right") {
			rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.3f, 0.55f));
		}
		else if (state == "left") {
			rotate = glm::rotate(identity, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.17f, -0.3f, 0.55f));
		}
		else {
			rotate = glm::rotate(identity, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			translate2 = glm::translate(identity, glm::vec3(0.0f, -0.0f, 0.0f));
		}

		translate = glm::translate(identity, glm::vec3(-0.17f, -0.3f, 0.0f));
		cubemodel2 = translate * cubemodel;
		cubemodel = translate2 * rotate * translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		cubemodel = cubemodel2;

		// feet
		this->amb = glm::vec3(0.07f, 0.11f, 0.56f);
		this->diff = glm::vec3(0.07f, 0.11f, 0.56f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);


		if (state == "right") {
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.1f, 0.5f));
		}
		else if (state == "left") {
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.1f, -0.5f));
		}
		else {
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
		}

		scale = glm::scale(identity, glm::vec3(0.1f, 0.1f, 0.15f));
		translate = glm::translate(identity, glm::vec3(0.04f, -1.5f, -0.05f));
		cubemodel2 = translate * scale * identity;
		cubemodel = translate2 * translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		cubemodel = cubemodel2;

		if (state == "right") {
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.1f, -0.5f));
		}
		else if (state == "left") {
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.1f, 0.5f));
		}
		else {
			translate2 = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
		}

		translate = glm::translate(identity, glm::vec3(-0.17f, 0.0f, 0.0f));
		cubemodel = translate2 * translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// eyes
		this->amb = glm::vec3(0.07f, 0.11f, 0.56f);
		this->diff = glm::vec3(0.07f, 0.11f, 0.56f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		scale = glm::scale(identity, glm::vec3(0.1f, 0.05f, 0.08f));
		translate = glm::translate(identity, glm::vec3(-0.1f, -0.1f, 0.07f));
		cubemodel = translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.12f, 0.0f, 0.00f));
		cubemodel = translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// ears
		this->amb = glm::vec3(0.07f, 0.11f, 0.56f);
		this->diff = glm::vec3(0.07f, 0.11f, 0.56f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		scale = glm::scale(identity, glm::vec3(0.02f, 0.2f, 0.02f));
		translate = glm::translate(identity, glm::vec3(-0.16f, -0.1f, 0.0f));
		cubemodel = translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		translate = glm::translate(identity, glm::vec3(0.3f, 0.0f, 0.00f));
		cubemodel = translate * cubemodel;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);

		// hat
		this->amb = glm::vec3(0.07f, 0.11f, 0.56f);
		this->diff = glm::vec3(0.07f, 0.11f, 0.56f);
		this->spec = glm::vec3(0.0, 0.0, 0.0);

		scale = glm::scale(identity, glm::vec3(0.1f, 0.05f, 0.08f));
		translate = glm::translate(identity, glm::vec3(-0.05f, 0.05f, -0.02f));
		cubemodel = translate * scale * identity;
		modelTogether = alTogether * cubemodel;
		cube.drawCubeWithMaterialisticProperty(shader, this->amb, this->diff, this->spec, this->shininess, modelTogether);
	}

	void move(Shader& shader, glm::mat4 alTogether)
	{
		drawProtagonist(shader, alTogether);
	}
};

#endif /* character_h */