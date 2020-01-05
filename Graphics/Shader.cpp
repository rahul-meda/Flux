#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

unsigned int Shader::CompileShader(const GLenum type, const char* src)
{
	int success;
	char infoLog[512];

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, 0);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, 0, infoLog);
		std::cout << "ERROR::Shader compilation failed\n" << infoLog << std::endl;
	}

	return shader;
}

std::string Shader::ReadFile(const std::string& file)
{
	std::string src;
	std::ifstream ifs;

	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		ifs.open(file);

		std::stringstream stream;

		stream << ifs.rdbuf();
		ifs.close();

		return stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to read file: " << file << std::endl;
	}
}

unsigned int Shader::CreateShader(const char* vertShaderPath, const char* fragShaderPath)
{
	std::string src = ReadFile(vertShaderPath);
	const char* cstr = &src[0];
	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, cstr);

	src = ReadFile(fragShaderPath);
	cstr = &src[0];
	unsigned int fragShader = CompileShader(GL_FRAGMENT_SHADER, cstr);

	int success;
	char infoLog[512];

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::Shader program linking failed\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}