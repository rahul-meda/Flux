#pragma once

#include <glad/glad.h>
#include <string>

namespace Shader
{
	unsigned int CompileShader(const GLenum type, const char* src);

	std::string ReadFile(const std::string& file);
	
	unsigned int CreateShader(const char* vertShaderPath, const char* fragShaderPath);
}