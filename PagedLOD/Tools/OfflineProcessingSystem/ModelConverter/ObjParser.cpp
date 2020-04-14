#include "ObjParser.h"
#include "nv_dds.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

CObjParser::CObjParser(const std::string& vFilePath)
{
	__processObj(vFilePath);
}

//****************************************************************************
//FUNCTION:
void CObjParser::__processObj(const std::string& vFilePath)
{
	const auto BackLashIndex = static_cast<unsigned int>(vFilePath.find_last_of('\\') + 1);
	const auto DotIndex = static_cast<unsigned int>(vFilePath.find_last_of('.'));
	m_Directory = vFilePath.substr(0, BackLashIndex);
	m_Name = vFilePath.substr(BackLashIndex, DotIndex - BackLashIndex);
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(vFilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::FILE:: " << vFilePath << importer.GetErrorString();
		return;
	}

	__processNode(scene->mRootNode, scene);
}

//****************************************************************************
//FUNCTION:
void CObjParser::__processNode(aiNode* vNode, const aiScene* vScene)
{
	for (unsigned int i = 0; i < vNode->mNumMeshes; i++)
	{
		aiMesh* mesh = vScene->mMeshes[vNode->mMeshes[i]];
		__processMesh(mesh, vScene);
	}
	for (unsigned int i = 0; i < vNode->mNumChildren; i++)
	{
		__processNode(vNode->mChildren[i], vScene);
	}
}

//****************************************************************************
//FUNCTION:
void CObjParser::__processMesh(aiMesh* vMesh, const aiScene* vScene)
{
	std::vector<hivePagedLOD::SOSGVertex> VertexSet;
	for (unsigned int i = 0; i < vMesh->mNumVertices; i++)
	{
		hivePagedLOD::SOSGVertex Vertex;

		Vertex.Position[0] = vMesh->mVertices[i].x;
		Vertex.Position[1] = vMesh->mVertices[i].y;
		Vertex.Position[2] = vMesh->mVertices[i].z;

		if (vMesh->mTextureCoords[0])
		{
			Vertex.TexCoord[0] = vMesh->mTextureCoords[0][i].x;
			Vertex.TexCoord[1] = vMesh->mTextureCoords[0][i].y;
		}
		VertexSet.push_back(Vertex);
	}

	std::vector<unsigned int> IndexSet;
	for (unsigned int i = 0; i < vMesh->mNumFaces; i++)
	{
		aiFace Face = vMesh->mFaces[i];

		for (unsigned int j = 0; j < Face.mNumIndices; j++)
			IndexSet.push_back(Face.mIndices[j]);
	}

	m_GeometrySet.emplace_back(VertexSet, IndexSet);

	if (!m_TextureHasBeenLoaded && vMesh->mMaterialIndex >= 0)
	{
		aiMaterial* Material = vScene->mMaterials[vMesh->mMaterialIndex];

		__loadTexture(Material, aiTextureType_DIFFUSE, m_Texture);
	}
}

//****************************************************************************
//FUNCTION:
void CObjParser::__loadTexture(aiMaterial* vMat, aiTextureType vType, hivePagedLOD::STexture& voTexture)
{
	aiString Str;
	vMat->GetTexture(vType, 0, &Str);

	_ASSERTE(Str.C_Str());

	std::string TextureFilePath = m_Directory + Str.C_Str();
	nv_dds::CDDSImage DDSImage;
	DDSImage.load(TextureFilePath);

	voTexture.TexInfo.Width = DDSImage.get_width();
	voTexture.TexInfo.Height = DDSImage.get_height();
	voTexture.TexInfo.Size = DDSImage.get_size();
	voTexture.pTextureData = std::shared_ptr<unsigned char>(new unsigned char[DDSImage.get_size()]);
	memcpy(voTexture.pTextureData.get(), DDSImage, DDSImage.get_size());
	//notice:
	//INTERNAL FORMAT
	voTexture.TexInfo.InternalFormat = 0;
}