#pragma once
#include "FrameState.h"
#include "GPUBufferCommon.h"

#include <memory>
namespace hivePagedLOD
{
	class IGPUBufferInterface
	{
	public:
		IGPUBufferInterface() = default;
		virtual ~IGPUBufferInterface() = default;
		virtual TTextureID generateTextureBufferForOSGV(const std::shared_ptr<const STexture>& vTexture) = 0;
		virtual std::pair<SGPUGeometryBufferHandle, unsigned int> generateGeometryBufferForOSGV(const std::shared_ptr<const SGeometry>& vGeometry) = 0;
		virtual bool destroyGeometryBufferForOSGV(const SGPUGeometryBufferHandle& vGPUGeometryHandle) = 0;
		virtual bool destroyTextureBufferForOSGV(TTextureID TextureID) = 0;
		virtual void initV(unsigned int vWidth, unsigned int vHeight) = 0;
		virtual std::shared_ptr<IFrameState> updateFrameStateV() = 0;
		virtual void prepareGUIV() = 0;
		virtual void renderV(const std::vector<std::shared_ptr<SGPUMeshBuffer>>& vTileSet) = 0;
		virtual void setCameraDefaultPositionV(float vX, float vY, float vZ) = 0;
		virtual void initFirstFrameStateV(const std::shared_ptr<IFrameState>& vFrameState) = 0;
		virtual bool windowShouldCloseV() = 0;
		virtual void endV() = 0;
		virtual void saveCaptureFlagV(bool vFlag) = 0;
		virtual void saveCapture2FileV(const std::string& vPath, unsigned int vWidth, unsigned int vHeight) = 0;
	};
}