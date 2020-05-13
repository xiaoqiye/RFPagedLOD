#include "Utils.h"
#include "Common.h"
#include "Timer.h"
#include "TileNodeLoader.h"
#include "Scene.h"
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <string>
#include <windows.h>
#include <iostream>

using namespace hivePagedLOD;

CTileNodeLoader::CTileNodeLoader(const std::shared_ptr<CPipelineRenderingTileNodeGenerator2TileNodeLoader>& vPipelineInputFromMemoryRendering, const std::shared_ptr<CPipelineTileNodeLoader2MemoryBufferManager>& vPipelineOutputToMemoryBufferManager)
	: m_pPipelineInputFromRendering(vPipelineInputFromMemoryRendering),
	m_pPipelineOutputToMemoryBufferManager(vPipelineOutputToMemoryBufferManager)
{
	_ASSERTE(m_pPipelineOutputToMemoryBufferManager && m_pPipelineInputFromRendering);
}

//****************************************************************************
//FUNCTION:
void CTileNodeLoader::start()
{
	m_Thread = std::thread([&] {__workByRenderingTileNodeSet(); });
}

//****************************************************************************
//FUNCTION:load all root
void CTileNodeLoader::doFirstFrame()
{
	std::vector<std::shared_ptr<SMemoryMeshBuffer>> MeshBufferSet;
	std::vector<unsigned int> EmptyDrawUIDSet;
	CTimer::getInstance()->tick(__FUNCTION__);

	std::vector<unsigned int> TileNodeUIDSet;
	for (auto& Root : CScene::getInstance()->getTileSet())
	{
		if (!Root) continue;
		auto GeoName = Root->getGeometryFileName();
		//FIXME:判断是否正确进行了处理，若输入正确，则直接返回输入
		auto TexName = CUtils::getInstance()->getTextureFileNameFromGeometryFileName(GeoName);
		std::shared_ptr<STexture> pTexture = __loadTextureBufferFromFile(TexName);
		pTexture->TextureFileName = TexName;

		std::shared_ptr<SGeometry> pGeometry = __loadGeometryBufferFromFile(GeoName);
		pGeometry->GeometryFileName = GeoName;
		auto pMemoryMeshBuffer = std::make_shared<SMemoryMeshBuffer>();

		pMemoryMeshBuffer->pTexture = pTexture;
		pMemoryMeshBuffer->pGeometry = pGeometry;
		m_RecentLoadGeometryNameSet.insert(GeoName);
		MeshBufferSet.emplace_back(pMemoryMeshBuffer);
		TileNodeUIDSet.emplace_back(Root->getUID());
	}
	
	CTimer::getInstance()->tock(__FUNCTION__);
	if (CTimer::getInstance()->needOutput() && CTimer::getInstance()->isRegistered(__FUNCTION__))
		std::cout << __FUNCTION__ << CTimer::getInstance()->getCostTimeByName(__FUNCTION__) << std::endl;
	
	m_pPipelineOutputToMemoryBufferManager->tryPush(std::make_shared<SLoaderResult>(TileNodeUIDSet, MeshBufferSet, EmptyDrawUIDSet));
}

//****************************************************************************
//FUNCTION:
void CTileNodeLoader::__workByRenderingTileNodeSet()
{
	while (!m_Close)
	{
		std::shared_ptr<SRenderingGeneratorResult> RenderingGeneratorResult;
		if (!m_Close && m_pPipelineInputFromRendering->tryPop(1, RenderingGeneratorResult))
			__processLoadTask(RenderingGeneratorResult);
	}
}

//****************************************************************************
//FUNCTION:
void CTileNodeLoader::__processLoadTask(const std::shared_ptr<SRenderingGeneratorResult>& vRenderingGeneratorResult)
{
	m_LoadedNumThisFrame = 0;
	m_LoadedSizeThisFrame = 0;
	m_LoadedTimeThisFrame = 0;

	if (m_UseLoadLog && m_LoadLogSet.size() > m_MaxLogNum)
		m_LoadLogSet.clear();

	++m_LoadFunctionCallTimes;
	if (m_ClearLoadSetLimit < m_LoadFunctionCallTimes)
	{
		m_RecentLoadGeometryNameSet.clear();
		m_LoadFunctionCallTimes = 0;
	}
	
	m_ThisLoadTaskTextureNameSet.clear();
	std::vector<std::shared_ptr<SMemoryMeshBuffer>> MeshBufferSet;
	std::vector<unsigned int> TileNodeUIDSet;
	for (auto& LoadTask : vRenderingGeneratorResult->LoadTaskSet) 
		__LoadFromFile(LoadTask, MeshBufferSet, TileNodeUIDSet);
	
	m_pPipelineOutputToMemoryBufferManager->tryPush(std::make_shared<SLoaderResult>(TileNodeUIDSet, MeshBufferSet, vRenderingGeneratorResult->DrawUIDSet));

	if (m_UseLoadLog && m_LoadedNumThisFrame != 0)
		m_LoadLogSet.emplace_back(STileLoadLog{ m_LoadedNumThisFrame, m_LoadedSizeThisFrame, CTimer::getInstance()->getCostTimeByName(__FUNCTION__) });

	//加载信息记录，结束时写到文件
	if (m_LoadedNumThisFrame != 0 && vRenderingGeneratorResult->IsKnapsackResult)
	{
		double CostTime = m_LoadedTimeThisFrame;

		m_LoadedNumThisK += m_LoadedNumThisFrame;
		m_LoadedSizeThisK += m_LoadedSizeThisFrame;
		m_LoadedTimeThisK += CostTime;

		std::vector<double> TempPerFrameInfo = { static_cast<double>(m_KnapsackIndex), static_cast<double>(m_LoadedNumThisFrame), static_cast<double>(m_LoadedSizeThisFrame), CostTime };
		m_PerFrameKnapsackLoadInfo.emplace_back(TempPerFrameInfo);
	}

	if (m_LoadedNumThisFrame == 0 && vRenderingGeneratorResult->IsKnapsackEnd)
	{
		std::vector<double> TempInfo = { static_cast<double>(vRenderingGeneratorResult->FrameID), static_cast<double>(m_LoadedNumThisK), static_cast<double>(m_LoadedSizeThisK), m_LoadedTimeThisK };
		m_AllKnapsackLoadInfo.push_back(TempInfo);

		m_LoadedNumThisK = 0;
		m_LoadedSizeThisK = 0;
		m_LoadedTimeThisK = 0;

		++m_KnapsackIndex;
	}
	//加载信息记录，结束时写到文件
}

//****************************************************************************
//FUNCTION:
void CTileNodeLoader::__LoadFromFile(const std::shared_ptr<SLoadTask>& vLoadTask, std::vector<std::shared_ptr<SMemoryMeshBuffer>>& voMeshBufferSet, std::vector<unsigned int>& voTileNodeUIDSet)
{
	auto& GeoName = vLoadTask->TileNode->getGeometryFileName();
	if (m_RecentLoadGeometryNameSet.find(GeoName) != m_RecentLoadGeometryNameSet.end())
		return;

	const auto pGeometry = __loadGeometryBufferFromFile(GeoName);
	pGeometry->GeometryFileName = GeoName;
	m_RecentLoadGeometryNameSet.insert(GeoName);
	
	auto& TexName = vLoadTask->TileNode->getTextureFileName();
	std::shared_ptr<STexture> pTexture = nullptr;
	if (m_ThisLoadTaskTextureNameSet.find(TexName) == m_ThisLoadTaskTextureNameSet.end() && vLoadTask->NeedLoadTexture)
	{
		pTexture = __loadTextureBufferFromFile(TexName);
		m_ThisLoadTaskTextureNameSet.insert(TexName);
	}
	
	auto pMemoryMeshBuffer = std::make_shared<SMemoryMeshBuffer>();
	pMemoryMeshBuffer->pTexture = pTexture;
	pMemoryMeshBuffer->pGeometry = pGeometry;
	voMeshBufferSet.emplace_back(pMemoryMeshBuffer);
	voTileNodeUIDSet.emplace_back(vLoadTask->TileNode->getUID());
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<char> CTileNodeLoader::__getFileStream(const std::string& vFilePath)
{
	_ASSERTE(!vFilePath.empty());
	//notice:FILE 效率比ifstream高一点点
	FILE* fp;
	fopen_s(&fp, vFilePath.c_str(), "rb");
	unsigned int FileSize;
	fread(&FileSize,sizeof(unsigned int), 1, fp);
	auto pFileStream = std::shared_ptr<char>(reinterpret_cast<char*>(std::malloc(FileSize)));
	fread(pFileStream.get(), sizeof(char), FileSize, fp);	
	fclose(fp);

	//std::ifstream InputFile(vFilePath, std::ios::binary | std::ios::in);
	//_ASSERTE(InputFile.is_open());
	//unsigned int FileSize;
	//InputFile.read(reinterpret_cast<char*>(&FileSize), sizeof(unsigned int));
	//auto pFileStream = std::shared_ptr<char>(reinterpret_cast<char*>(std::malloc(FileSize)));
	//InputFile.read(pFileStream.get(), FileSize);
	//InputFile.close();

	if (m_UseLoadLog)
		m_LoadedSizeThisFrame += FileSize;

	return pFileStream;
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<SGeometry> CTileNodeLoader::__loadGeometryBufferFromFile(const std::string& vFilePath)
{
	if (m_UseLoadLog)
		++m_LoadedNumThisFrame;

	CTimer::getInstance()->tick(__FUNCTION__);
	const auto pFileStream = __getFileStream(vFilePath);
	CTimer::getInstance()->tock(__FUNCTION__);
	m_LoadedTimeThisFrame += CTimer::getInstance()->getCostTimeByName(__FUNCTION__);

	auto pGeometry = std::make_shared<SGeometry>();
	if (pFileStream) __processGeometryBuffer(pFileStream.get(), pGeometry);
	return pGeometry;
}

//****************************************************************************
//FUNCTION:
std::shared_ptr<STexture> CTileNodeLoader::__loadTextureBufferFromFile(const std::string& vFilePath)
{
	if (m_UseLoadLog)
		++m_LoadedNumThisFrame;

	CTimer::getInstance()->tick(__FUNCTION__);
	auto pFileStream = __getFileStream(vFilePath);
	CTimer::getInstance()->tock(__FUNCTION__);
	m_LoadedTimeThisFrame += CTimer::getInstance()->getCostTimeByName(__FUNCTION__);

	auto pTexture = std::make_shared<STexture>();
	pTexture->TextureFileName = vFilePath;
	if (pFileStream) __processTextureBuffer(pFileStream.get(), pTexture);
	return pTexture;
}

//****************************************************************************
//FUNCTION:
void CTileNodeLoader::__processGeometryBuffer(const void* vGeoBufferStream, std::shared_ptr<SGeometry>& voGeometry) const
{
	const unsigned int VertexBufferSize = reinterpret_cast<const unsigned int*>(vGeoBufferStream)[0];
	std::shared_ptr<char> pVertexBuffer(new char[VertexBufferSize]);
	const char* pVertexBufferStream = reinterpret_cast<const char*>(vGeoBufferStream) + sizeof(unsigned int);
	std::memcpy(pVertexBuffer.get(), pVertexBufferStream, VertexBufferSize);

	const unsigned int IndexBufferSize = reinterpret_cast<const unsigned int*>(pVertexBufferStream + VertexBufferSize)[0];
	std::shared_ptr<char> pIndexBuffer(new char[IndexBufferSize]);
	const char* pIndexBufferStream = reinterpret_cast<const char*>(pVertexBufferStream) + VertexBufferSize + sizeof(unsigned int);
	std::memcpy(pIndexBuffer.get(), pIndexBufferStream, IndexBufferSize);

	voGeometry->VertexCount = VertexBufferSize / sizeof(OSG_VERTEX_SIZE);
	voGeometry->pVertexData = std::reinterpret_pointer_cast<float>(pVertexBuffer);
	voGeometry->IndexCount = IndexBufferSize / sizeof(unsigned int);
	voGeometry->pIndexData = std::reinterpret_pointer_cast<unsigned int>(pIndexBuffer);
}

//****************************************************************************
//FUNCTION:
void CTileNodeLoader::__processTextureBuffer(const void* vTexBufferStream, std::shared_ptr<STexture>& voTexture) const
{
	std::memcpy(&voTexture->TexInfo, vTexBufferStream, sizeof(STextureInfo));
	const unsigned int TextureDataBufferOffset = sizeof(STextureInfo);
	const char* pTextureBufferStream = reinterpret_cast<const char*>(vTexBufferStream) + TextureDataBufferOffset;

	const unsigned int TextureBufferSize = voTexture->TexInfo.Size;
	std::shared_ptr<unsigned char> TextureBuffer(new unsigned char[TextureBufferSize]);
	std::memcpy(TextureBuffer.get(), pTextureBufferStream, TextureBufferSize);

	voTexture->pTextureData = TextureBuffer;
}