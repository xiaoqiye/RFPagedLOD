#pragma once
#include "GLHeader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace OpenGL_LIB {

	class CShader
	{
	public:
		CShader();
		~CShader() = default;
		void addShader(const GLchar* shaderPath, GLuint shaderType);
		void linkShader();
		void useShader() { glUseProgram(getShaderID()); }

		void buildUniformLocationMap(const std::vector<std::string>& vUniformNames){}

		void setBool(const std::string &name, bool value);
		void setInt(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(getShaderID(), name.c_str()), value); }
		void setFloat(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(getShaderID(), name.c_str()), value); }

		void setVec2(const std::string &name, const glm::vec2 &value) const { glUniform2fv(glGetUniformLocation(getShaderID(), name.c_str()), 1, &value[0]); }
		void setVec2(const std::string &name, float x, float y) const { glUniform2f(glGetUniformLocation(getShaderID(), name.c_str()), x, y); }

		void setVec3(const std::string &name, const glm::vec3 &value, int num = 1) const { glUniform3fv(glGetUniformLocation(getShaderID(), name.c_str()), num, &value[0]); }
		void setVec3(const std::string &name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(getShaderID(), name.c_str()), x, y, z); }

		void setVec4(const std::string &name, const glm::vec4 &value) const { glUniform4fv(glGetUniformLocation(getShaderID(), name.c_str()), 1, &value[0]); }
		void setVec4(const std::string &name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(getShaderID(), name.c_str()), x, y, z, w); }

		void setMat2(const std::string &name, const glm::mat2 &mat) const { glUniformMatrix2fv(glGetUniformLocation(getShaderID(), name.c_str()), 1, GL_FALSE, &mat[0][0]); }
		void setMat3(const std::string &name, const glm::mat3 &mat) const { glUniformMatrix3fv(glGetUniformLocation(getShaderID(), name.c_str()), 1, GL_FALSE, &mat[0][0]); }
		void setMat4(const std::string &name, const glm::mat4 &mat) const { glUniformMatrix4fv(glGetUniformLocation(getShaderID(), name.c_str()), 1, GL_FALSE, &mat[0][0]); }

		unsigned int getShaderID() const { return mId; }

	private:
		unsigned int mId;
		std::vector<GLuint> mShaderSet;
		std::unordered_map<std::string, int> mUniformLocationMap;
		void __initShader();
	};
}
