#pragma once

#include <map>
#include <vector>

struct TextureInfo;

namespace Texture
{
	enum TextureType {WOOD, SMILEY};

	unsigned int CreateTexture(TextureType type, const char* filePath, bool alpha, TextureInfo& textureData);

	void AddUniformLoc(char* loc, TextureInfo& textureData);

	void SetUniforms(unsigned int shaderID, TextureInfo& textureData);
}

struct TextureInfo
{
	std::map<Texture::TextureType, unsigned int> textureMap;

	std::vector<unsigned int> textures;

	std::vector<char*> uniformLocs;
};

