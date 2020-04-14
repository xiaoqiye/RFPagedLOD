#include "Shader.h"
#include "ConsoleLog.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace OpenGL_LIB;

CShader::CShader() : mId(0)
{
	__initShader();
}

//****************************************************************************
//FUNCTION:
void CShader::addShader(const GLchar* shaderPath, GLuint shaderType)
{
	_ASSERTE(Utils::isFileExisted(shaderPath));
	std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::badbit);
	try {
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		_DEBUG_OUTPUT_2(e.what(), "can not read shader file.");
	}
	const GLchar* charShaderCode = shaderCode.c_str();
	GLint success;
	GLchar infoLog[512];
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &charShaderCode, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		_DEBUG_OUTPUT_3("shader compile error: ", infoLog, shaderPath);
	}
	glAttachShader(mId, shader);
	mShaderSet.push_back(shader);
}

//****************************************************************************
//FUNCTION:
void CShader::linkShader()
{
	glLinkProgram(mId);
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(mId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(mId, 512, NULL, infoLog);
		_DEBUG_OUTPUT_2("shader link error: ", infoLog);
	}
	for (GLuint shader : mShaderSet) {
		glDeleteShader(shader);
	}
}

//****************************************************************************
//FUNCTION:
void CShader::__initShader()
{
	mId = glCreateProgram();
#if _DEBUG
	if (mId == 0) 
		_DEBUG_OUTPUT("error creating shader program");
#endif
}

//****************************************************************************
//FUNCTION:
void CShader::setBool(const std::string &vName, bool vValue)
{
	if (mUniformLocationMap.count(vName) == 0)
		mUniformLocationMap[vName] = glGetUniformLocation(getShaderID(), vName.c_str());
	int location = mUniformLocationMap[vName];
	/*if (location < 0)
		_OUTPUT_LINE(vName + " does not correspond to an active uniform variable");*/
	glUniform1i(location, (int)vValue);
}