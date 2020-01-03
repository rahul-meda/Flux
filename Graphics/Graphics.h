#pragma once

#include <vector>

class Model;

class Graphics
{
private:
	Graphics();

public:
	std::vector<Model> models;

	static Graphics& GetInstance();

	void Update(int shaderProgram);
};