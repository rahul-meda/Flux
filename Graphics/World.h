#pragma once

#include <glm/mat4x4.hpp>
#include <vector>

unsigned int CreateTerrain();
void CreateEnvironmentMap(const std::string& path, unsigned int& envMap, unsigned int& irradianceMap, unsigned int& prefilterMap, unsigned int& brdfLUTTexture);
void CreateSkybox (unsigned int& VAO, unsigned int& textureID);
unsigned int CreateBox();
unsigned int CreateQuad();
void CreateGrass  (unsigned int& VAO, unsigned int& textureID);
void CreateTerrain(unsigned int& VAO, unsigned int& textureID);
void CreateTrees  (unsigned int& VAO, unsigned int& textureID);
void CreateAsteroidBelt(std::vector<glm::mat4>& transforms, const unsigned int count);

class SubMesh; class Material; class aiScene; class R_Vertex;
class I_Mesh
{
private:
	unsigned int VAO;
	std::vector<SubMesh> subMeshes;
	std::vector<Material> materials;
	unsigned int instanceCount;

	void LoadGeometry(const aiScene* scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices);
	void LoadTextures(const aiScene* scene, const std::string& file, bool flip = false); 
public:
	I_Mesh(const unsigned int instanceCount);
	void LoadModel(const std::string& fileName, bool flip = false);
	friend class Graphics;
};