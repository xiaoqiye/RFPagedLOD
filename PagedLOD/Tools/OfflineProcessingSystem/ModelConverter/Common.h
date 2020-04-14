#pragma once
#include <GLM/glm.hpp>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <GLM/gtc/matrix_transform.hpp>
#include <thread>

namespace hivePagedLOD
{
	struct SOSGVertex
	{
		float Position[3];
		float TexCoord[2];
	};
	struct STextureInfo
	{
		unsigned int Width = 0;
		unsigned int Height = 0;
		unsigned int InternalFormat = 0;
		unsigned int Size = 0;
	};
	struct STexture
	{
		std::string TextureFileName;
		STextureInfo TexInfo;
		std::shared_ptr<unsigned char> pTextureData = nullptr;
		bool isValid() const { return TexInfo.Size > 0; }
	};
	const std::string BINARY_TEXTURE_SUFFIX = ".bint";
	const std::string BINARY_GEOMETRY_SUFFIX = ".bin";
}
