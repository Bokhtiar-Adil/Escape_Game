#ifndef  cylinder_h
#define cylinder_h

#define M_PI 3.14159265358979323846

#include <glad/glad.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Shader.h"


using namespace std;

class Cylinder
{
private:
	unsigned int VAO, VBO, EBO, texture;
	std::string texturePath;
	int indexCount;
    float shininess = 32.0f;

    unsigned int loadTexture(const char* path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Set texture wrapping and filtering options
        // ...

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cerr << "Texture failed to load at path: " << path << std::endl;
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureID;
    }

	void setupCylinder(float r1, float r2, float height, int angularSegments, int heightwiseSegments)
	{
        std::vector<GLfloat> vertexData;
        std::vector<unsigned int> indexData;

        float deltaAngle = 2.0f * M_PI / angularSegments;
        float deltaHeight = height / heightwiseSegments;
        glm::vec2 textureDelta = glm::vec2(1.0f / angularSegments, 1.0f / heightwiseSegments);

        for (int i = 0; i <= heightwiseSegments; ++i) {
            float currentHeight = -0.5f * height + i * deltaHeight;
            float radius = r1 + (r2 - r1) * (currentHeight + 0.5f * height) / height;

            for (int j = 0; j <= angularSegments; ++j) {
                float currentAngle = j * deltaAngle;
                float x = radius * cos(currentAngle);
                float y = currentHeight;
                float z = radius * sin(currentAngle);

                // Position
                vertexData.push_back(x);
                vertexData.push_back(y);
                vertexData.push_back(z);

                // Normal
                glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
                vertexData.push_back(normal.x);
                vertexData.push_back(normal.y);
                vertexData.push_back(normal.z);

                // Texture Coordinate
                vertexData.push_back(j * textureDelta.x);
                vertexData.push_back(i * textureDelta.y);
            }
        }

        for (int i = 0; i < heightwiseSegments; ++i) {
            for (int j = 0; j < angularSegments; ++j) {
                int first = i * (angularSegments + 1) + j;
                int second = first + angularSegments + 1;
                
                indexData.push_back(first);
                indexData.push_back(second);
                indexData.push_back(first + 1);
                
                indexData.push_back(second);
                indexData.push_back(second + 1);
                indexData.push_back(first + 1);
            }
        }
        indexCount = indexData.size();

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        // Texture coordinates attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        texture = loadTexture(texturePath.c_str());
	}

public:
    Cylinder()
    {
        this->indexCount = 0;
    }

    void setupCylinderProperties(float r1, float r2, float height, int angularSegments, int heightwiseSegments, const std::string& texturePath)
    {
        this->texturePath = texturePath;        

        setupCylinder(r1, r2, height, angularSegments, heightwiseSegments);
    }

    void drawCylinder(Shader& shader, const glm::mat4& transform) {

        shader.use();
        shader.setBool("exposedToSun", true);

        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 0);
        shader.setFloat("material.shininess", this->shininess);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader.setMat4("model", transform);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Cylinder()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteTextures(1, &texture);
    }
};

#endif // ! cylinder_h