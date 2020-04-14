#include "FileUtils.h"
#include "ObjParser.h"
#include "Common.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

//****************************************************************************
//FUNCTION:
void FileUtils::generateFile(const CObjParser& vObjParser)
{
	std::string Directory = vObjParser.getDirectory();
	unsigned int index = static_cast<unsigned int>(Directory.find_first_of(ObjFilePath) + ObjFilePath.length());
	Directory = BinFilePath + Directory.substr(index, Directory.size() - index);
	if (!boost::filesystem::exists(Directory))
		boost::filesystem::create_directories(Directory);

	for(unsigned int i = 0; i < vObjParser.getGeometryDataCount(); ++i)
	{
		generateGeometryFile(vObjParser.getGeometryData(i), Directory, vObjParser.getName() + '_' + std::to_string(i));
	}

	generateTextureFile(vObjParser.getTexture(), Directory, vObjParser.getName());
}

//****************************************************************************
//FUNCTION:
void FileUtils::generateTextureFile(const hivePagedLOD::STexture& vTextureData, const std::string& vDirectory, const std::string& vName)
{
	char* pTextureBuffer = nullptr;
	unsigned int TotalSize = 0;
	buildTextureBuffer(vTextureData, &pTextureBuffer, TotalSize);
	_ASSERTE(pTextureBuffer && TotalSize > 0);

	std::string OutputFilePath = vDirectory + vName + hivePagedLOD::BINARY_TEXTURE_SUFFIX;
	bool MakeFileSucceed = makeFile(OutputFilePath, pTextureBuffer, TotalSize + sizeof(unsigned int));

	if (MakeFileSucceed)
		std::cout << OutputFilePath << "\t is done" << std::endl;
	else
		std::cout << OutputFilePath << "\t is not be done" << std::endl;

	delete[] pTextureBuffer;
}

//****************************************************************************
//FUNCTION:
void FileUtils::generateGeometryFile(const CGeometryData& vGeometryData, const std::string& vDirectory, const std::string& vName)
{
	char* pGeometryBuffer = nullptr;
	unsigned int TotalSize = 0;
	buildGeometryBuffer(vGeometryData, &pGeometryBuffer, TotalSize);
	_ASSERTE(pGeometryBuffer && TotalSize > 0);

	std::string OutputFilePath = vDirectory + vName + ".bin";
	bool MakeFileSucceed = makeFile(OutputFilePath, pGeometryBuffer, TotalSize + sizeof(unsigned int));

	if(MakeFileSucceed)
		std::cout << OutputFilePath << "\t is done" << std::endl;
	else
		std::cout << OutputFilePath << "\t is not be done" << std::endl;

	delete[] pGeometryBuffer;
}

//****************************************************************************
//FUNCTION:
bool FileUtils::makeFile(const std::string& vOutputFilePath, const char* vBuffer, unsigned int vSize)
{
	if (boost::filesystem::exists(vOutputFilePath))
		return false;
	std::ofstream BinFile(vOutputFilePath, std::ios::binary | std::ios::out);
	_ASSERTE(BinFile.is_open());

	BinFile.write(vBuffer, vSize);
	BinFile.close();
	return true;
}

//****************************************************************************
//FUNCTION:
void FileUtils::fillBuffer(char** voBufferOffset, const void* vData, unsigned int vDataSize)
{
	memcpy(*voBufferOffset, vData, vDataSize);
	*voBufferOffset += vDataSize;
}

//****************************************************************************
//FUNCTION:
void FileUtils::buildTextureBuffer(const hivePagedLOD::STexture& vTexture, char** voMeshBuffer, unsigned int& voTotalSize)
{
	unsigned int SizeOfUint = sizeof(unsigned int);

	unsigned int TextureSize = vTexture.TexInfo.Size;
	voTotalSize = sizeof(hivePagedLOD::STextureInfo) + TextureSize;

	*voMeshBuffer = new char[SizeOfUint + voTotalSize];
	char* WriteIndex = *voMeshBuffer;

	fillBuffer(&WriteIndex, &voTotalSize, SizeOfUint);
	fillBuffer(&WriteIndex, &vTexture.TexInfo, sizeof(hivePagedLOD::STextureInfo));
	fillBuffer(&WriteIndex, vTexture.pTextureData.get(), TextureSize);
}

//****************************************************************************
//FUNCTION:
void FileUtils::buildGeometryBuffer(const CGeometryData& vGeometryData, char** voMeshBuffer, unsigned int& voTotalSize)
{
	unsigned int SizeOfUint = sizeof(unsigned int);

	unsigned int VertexBufferSize = sizeof(hivePagedLOD::SOSGVertex) * vGeometryData.getVertexSetSize();
	unsigned int IndexBufferSize = SizeOfUint * vGeometryData.getIndexSetSize();
	voTotalSize = SizeOfUint * 2 + VertexBufferSize + IndexBufferSize;

	*voMeshBuffer = new char[SizeOfUint + voTotalSize];
	char* WriteIndex = *voMeshBuffer;

	fillBuffer(&WriteIndex, &voTotalSize, SizeOfUint);
	fillBuffer(&WriteIndex, &VertexBufferSize, SizeOfUint);
	fillBuffer(&WriteIndex, vGeometryData.getVertexData(), VertexBufferSize);
	fillBuffer(&WriteIndex, &IndexBufferSize, SizeOfUint);
	fillBuffer(&WriteIndex, vGeometryData.getIndexData(), IndexBufferSize);
}