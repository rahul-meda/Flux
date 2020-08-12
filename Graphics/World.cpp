
#include <glad/glad.h>
#include <iostream>
#include "World.h"
#include "Graphics.h"
#include "../stb_image.h"

void CreateSkybox(unsigned int& VAO, unsigned int& textureID)
{
	float vertices[] = 
	{         
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,
		1.0f ,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f , -1.0f, -1.0f,
		1.0f , -1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f , -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f ,  1.0f, -1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f , -1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f , -1.0f,  1.0f
	};

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	const char* paths[6] = {"resources/textures/skybox/right.jpg",
							"resources/textures/skybox/left.jpg", 
							"resources/textures/skybox/top.jpg", 
							"resources/textures/skybox/bottom.jpg", 
							"resources/textures/skybox/front.jpg", 
							"resources/textures/skybox/back.jpg"};

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char *data = stbi_load(paths[i], &width, &height, &nrChannels, 0);
		if (!data)
		{
			std::cout << "Cubemap texture failed to load at path: " << paths[i] << std::endl;
			stbi_image_free(data);
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}