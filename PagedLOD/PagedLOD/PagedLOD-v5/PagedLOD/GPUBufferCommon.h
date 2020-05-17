#pragma once
#include "Common.h"

namespace hivePagedLOD
{
	typedef uint64_t TTextureID;
	struct SGPUTextureBuffer
	{
		SGPUTextureBuffer() = delete;
		SGPUTextureBuffer(const TTextureID& vTextureID) : TextureID(vTextureID) {}
		virtual ~SGPUTextureBuffer() = default;
		TTextureID getTextureID() const { return TextureID; }
		bool isValid() const { return TextureID != 0; }

		TTextureID TextureID = 0;
	};

	typedef uint64_t TVertexArrayHandle;
	typedef uint64_t TBufferHandle;
	struct SGPUGeometryBufferHandle
	{
		TVertexArrayHandle VAO = 0;
		TBufferHandle VBO = 0;
		TBufferHandle IBO = 0;
	};
	struct SGPUGeometryBuffer
	{
		SGPUGeometryBuffer() = delete;
		SGPUGeometryBuffer(const SGPUGeometryBufferHandle& vGeometryHandle, unsigned int vIndexCount) : GeometryHandle(vGeometryHandle), IndexCount(vIndexCount) {}
		virtual ~SGPUGeometryBuffer() = default;
		TVertexArrayHandle getVAO() const { return GeometryHandle.VAO; }
		unsigned int getIndexCount() const { return IndexCount; }
		bool isValid() const { return GeometryHandle.VAO != 0 && GeometryHandle.VBO != 0 && GeometryHandle.IBO != 0; }
		SGPUGeometryBufferHandle GeometryHandle;
		unsigned int IndexCount = 0;
	};

	struct SGPUMeshBuffer : SMeshBuffer
	{
		std::shared_ptr<SGPUTextureBuffer> pGPUTextureBuffer;
		std::shared_ptr<SGPUGeometryBuffer> pGPUGeometryBuffer;
		//add:show LODLevel
		int LODLevel = 0;
		SGPUMeshBuffer() = default;
		SGPUMeshBuffer(const std::shared_ptr<SGPUTextureBuffer>& vGPUTex, const std::shared_ptr<SGPUGeometryBuffer>& vGPUGeo, int vLODLevel)
		{
			pGPUTextureBuffer = vGPUTex;
			pGPUGeometryBuffer = vGPUGeo;
			LODLevel = vLODLevel;
		}
		bool isValidV() const override
		{
			return pGPUTextureBuffer->isValid() && pGPUGeometryBuffer->isValid();
		}
	};
}
