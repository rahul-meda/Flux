#include "Texture.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <iostream>

unsigned int Texture::CreateTexture(TextureType type, const char* filePath, bool alpha, TextureInfo& textureData)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
		return 0;
	}
	if (alpha)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	textureData.textures.push_back(texture);
	textureData.textureMap.insert(std::pair<TextureType, unsigned int>(type, texture));

	return texture;
}

void Texture::AddUniformLoc(char* loc, TextureInfo& textureData)
{
	textureData.uniformLocs.push_back(loc);
}

void Texture::SetUniforms(unsigned int shaderID, TextureInfo& textureData)
{
	glUseProgram(shaderID);
	for (int i = 0; i < textureData.uniformLocs.size(); i++)
	{
		glUniform1i(glGetUniformLocation(shaderID, textureData.uniformLocs[i]), i);
	}
	glUseProgram(0);
}