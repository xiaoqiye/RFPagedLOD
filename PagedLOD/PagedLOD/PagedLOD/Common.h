#pragma once
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <thread>
#include <iostream>

#include "TileNode.h"

namespace hivePagedLOD
{
	typedef std::pair<std::string, std::vector<std::string>> TTextureGeometryNamePair;

	////view info
	struct SViewPort
	{
		unsigned int Width = 0;
		unsigned int Height = 0;
	};
	struct SCameraInfo
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Up = glm::vec3(0.0f);
		glm::vec3 Front = glm::vec3(0.0f);
		float FOV = 0.0f;
		float NearPlane = 0.0f;
		float FarPlane = 0.0f;
	};
	struct SViewInfo
	{
		SViewPort ViewPortInfo;
		SCameraInfo CameraInfo;
	};

	////memory geometry texture
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
	struct SGeometry
	{
		std::string GeometryFileName;
		unsigned int VertexCount = 0;
		unsigned int IndexCount = 0;
		std::shared_ptr<float> pVertexData = nullptr;
		std::shared_ptr<unsigned int> pIndexData = nullptr;
	};

	////mesh buffer
	struct SMeshBuffer
	{
		SMeshBuffer() = default;
		virtual ~SMeshBuffer() = default;
		virtual bool isValidV() const = 0;
	};
	struct SMemoryMeshBuffer : SMeshBuffer
	{
		std::shared_ptr<SGeometry> pGeometry;
		std::shared_ptr<STexture> pTexture;
		SMemoryMeshBuffer()
		{
			pGeometry = std::make_shared<SGeometry>();
			pTexture = std::make_shared<STexture>();
		}
		bool isValidV() const override
		{
			return pGeometry->VertexCount > 0 &&
				pGeometry->IndexCount > 0 &&
				pTexture->TexInfo.Size > 0;
		}
	};

	////load task
	struct SLoadTask
	{
		std::shared_ptr<CTileNode> TileNode;
		bool NeedLoadTexture;
		SLoadTask() = default;
		SLoadTask(const std::shared_ptr<CTileNode>& vTileNode, bool vNeedLoadTexture) : TileNode(vTileNode), NeedLoadTexture(vNeedLoadTexture) {}
	};

	struct SRenderingGeneratorResult
	{
		std::vector<std::shared_ptr<SLoadTask>> LoadTaskSet;
		std::vector<unsigned int> DrawUIDSet;
		SRenderingGeneratorResult() = default;
		SRenderingGeneratorResult(const std::vector<std::shared_ptr<SLoadTask>>& vLoadTaskSet, const std::vector<unsigned int>& vDrawUIDSet) : LoadTaskSet(vLoadTaskSet), DrawUIDSet(vDrawUIDSet) {}
	};

	struct SLoaderResult
	{
		std::vector<unsigned int> TileNodeUIDSet;
		std::vector<std::shared_ptr<SMemoryMeshBuffer>> LoadedMeshBufferSet;
		std::vector<unsigned int> DrawUIDSet;
		SLoaderResult() = default;
		SLoaderResult(const std::vector<unsigned int>& vUIDSet, const std::vector<std::shared_ptr<SMemoryMeshBuffer>>& vLoadedMeshBufferSet, const std::vector<unsigned int>& vDrawUIDSet) : TileNodeUIDSet(vUIDSet), LoadedMeshBufferSet(vLoadedMeshBufferSet), DrawUIDSet(vDrawUIDSet) {}
	};
	
	////load log
	struct STileLoadLog
	{
		unsigned int LoadNum;
		std::uintmax_t LoadSize;
		double LoadTime;
		STileLoadLog(unsigned int vNum, std::uintmax_t vSize, double vTime) : LoadNum(vNum), LoadSize(vSize), LoadTime(vTime) {}
		std::string toString() const { return std::string(std::to_string(LoadNum) + "|" + std::to_string(LoadSize) + "|" + std::to_string(LoadTime)); }
	};

	////load cost
	struct STileNodeLoadCost
	{
		uintmax_t GeoSize;
		uintmax_t TexSize;
		bool GeoInMemory;
		bool TexInMemory;
		uintmax_t TriangleCount;
		uintmax_t LoadCost;

		STileNodeLoadCost() : GeoSize(0), TexSize(0), GeoInMemory(false), TexInMemory(false), TriangleCount(0), LoadCost(0) {}
		STileNodeLoadCost(uintmax_t vGeoSize, uintmax_t vTexSize, bool vGeoIn, bool vTexIn, uintmax_t vTriangleCount) : GeoSize(vGeoSize), TexSize(vTexSize), GeoInMemory(vGeoIn), TexInMemory(vTexIn), TriangleCount(vTriangleCount), LoadCost(GeoSize* static_cast<uintmax_t>(!GeoInMemory) + TexSize * static_cast<uintmax_t>(!TexInMemory)) {}
		STileNodeLoadCost(uintmax_t vGeoSize, uintmax_t vTexSize, uintmax_t vTriangleCount) : GeoSize(vGeoSize), TexSize(vTexSize), GeoInMemory(false), TexInMemory(false), TriangleCount(vTriangleCount), LoadCost(GeoSize + TexSize) {}

		void updateCost() { LoadCost = GeoSize * static_cast<uintmax_t>(!GeoInMemory) + TexSize * static_cast<uintmax_t>(!TexInMemory); }
		void outputCost() { std::cout << "GeoInMemory = " << GeoInMemory << " TexInMemory = " << TexInMemory << " LoadCost = " << LoadCost << std::endl; }
	};

	////knapsack
	struct SKnapsackItem
	{
		std::vector<std::shared_ptr<CTileNode>> TileNodeSet;
		uintmax_t LoadCostForKnapsackItem = 0;
		uintmax_t TriangleCount = 0;
		SKnapsackItem() = default;
		SKnapsackItem(const std::vector<std::shared_ptr<CTileNode>>& vTileNodeSet, uintmax_t vLoadCostForKnapsackItem, uintmax_t vTriangleCount)
			: TileNodeSet(vTileNodeSet), LoadCostForKnapsackItem(vLoadCostForKnapsackItem), TriangleCount(vTriangleCount) {};
	};

	struct SPreferredResult
	{
		std::vector<std::vector<std::shared_ptr<CTileNode>>> PreferredTileNodeSet;
		std::vector<unsigned int> MaxDeepSet;
		SPreferredResult() = default;
		SPreferredResult(const std::vector<std::vector<std::shared_ptr<CTileNode>>>& vPreferredTileNodeSet, const std::vector<unsigned int>& vMaxDeepSet) : PreferredTileNodeSet(vPreferredTileNodeSet), MaxDeepSet(vMaxDeepSet) {}
	};
	
	////constants
	const std::uintmax_t GEOMETRY_HEADER_LENGTH = 12;
	const std::uintmax_t TEXTURE_HEADER_LENGTH = 20;
	const std::uintmax_t OSG_VERTEX_SIZE = sizeof(float) * 5;

	const std::string BINARY_TEXTURE_SUFFIX = ".bint";
	const std::string BINARY_GEOMETRY_SUFFIX = ".bin";
	const unsigned int MAX_TILE_NODE_CHILD_NUM = 8;

	const glm::mat4 MODEL_MATRIX = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	const std::uintmax_t KILOBYTE = 1024;
	const std::uintmax_t MEGABYTE = 1024 * KILOBYTE;
	const std::uintmax_t GIGABYTE = 1024 * MEGABYTE;
	const std::uintmax_t DEFAULT_MEMORY_LIMIT = 2 * GIGABYTE;
	const std::uintmax_t DEFAULT_LIMIT_LOAD_SIZE = 1 * MEGABYTE;

	const unsigned int MAX_THREAD_COUNT = std::thread::hardware_concurrency();

	const std::string LOAD_STRATEGY_DO_NOTHING = "LOAD_STRATEGY_DO_NOTHING";
	const std::string LOAD_STRATEGY_LOAD_PARENT = "LOAD_STRATEGY_LOAD_PARENT";
	const std::string LOAD_STRATEGY_KNAPSACK = "LOAD_STRATEGY_KNAPSACK";
	const std::string LOAD_STRATEGY_LIMIT_LOAD = "LOAD_STRATEGY_LIMIT_LOAD";
	const std::string LOAD_STRATEGY_UNDEFINED = "LOAD_STRATEGY_UNDEFINED";
	const std::string LOAD_STRATEGY_LOAD_PARENT_ITEM = "LOAD_STRATEGY_LOAD_PARENT_ITEM";

	const unsigned int UID_TILE_NUM_MASK = 0xFF000000;
	const unsigned int OFFSET_BIT = 24;
	const unsigned int MAX_NODE_DEEP = 16;
}
