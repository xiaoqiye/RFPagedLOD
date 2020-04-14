#pragma once
#include "Common.h"
#include "GeometryData.h"
#include <assimp/material.h>
#include <assimp/scene.h>
#include <vector>

class CObjParser
{
public:
	CObjParser(const std::string& vObjFilePath);
	~CObjParser() = default;

	const hivePagedLOD::STexture& getTexture() const { return m_Texture; }
	const std::string& getDirectory() const { return m_Directory; }
	const std::string& getName() const { return m_Name; }
	const CGeometryData& getGeometryData(unsigned int vIndex) const { _ASSERTE(vIndex < m_GeometrySet.size()); return m_GeometrySet[vIndex]; }
	unsigned int getGeometryDataCount() const { return static_cast<unsigned int>(m_GeometrySet.size()); }

private:
	void __processObj(const std::string& vObjFilePath);
	void __processNode(aiNode* vNode, const aiScene* vScene);
	void __processMesh(aiMesh* vMesh, const aiScene* vScene);
	void __loadTexture(aiMaterial* vMat, aiTextureType vType, hivePagedLOD::STexture& voTexture);
	std::string m_Directory;
	std::string m_Name;
	bool m_TextureHasBeenLoaded = false;
	hivePagedLOD::STexture m_Texture;
	std::vector<CGeometryData> m_GeometrySet;
};