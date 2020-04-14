#pragma once
#include <string>
#include <vector>

class CGeometryData;

namespace hivePagedLOD 
{
	struct STexture;
}

//REVIEW: 不要有和hivePagedLOD平级的命名空间，都作为hivePagedLOD的子命名空间。
class CObjParser;

namespace FileUtils
{
	const std::string ObjFilePath = "Obj\\";
	const std::string BinFilePath = "Bin\\";

	void generateFile(const CObjParser& vObjParser); 
	static void generateTextureFile(const hivePagedLOD::STexture& vTextureData, const std::string& vDirectory, const std::string& vName);
	static void generateGeometryFile(const CGeometryData& vGeometryData, const std::string& vDirectory, const std::string& vName);
	static bool makeFile(const std::string& vOutputFilePath, const char* vBuffer, unsigned int vSize);
	static void buildTextureBuffer(const hivePagedLOD::STexture& vTexture, char** voMeshBuffer, unsigned int& voTotalSize);
	static void buildGeometryBuffer(const CGeometryData& vGeometryData, char** voMeshBuffer, unsigned int& voTotalSize);
	static void fillBuffer(char** voBufferOffset, const void* vData, unsigned int vDataSize);
}