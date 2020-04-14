#pragma once
#include "Common.h"

namespace hivePagedLOD
{
	enum class EGPUTaskType
	{
		INIT_WINDOW,
		GEN_GEOMETRY_BUFFER,
		GEN_TEXTURE_BUFFER,
		DEL_GEOMETRY_BUFFER,
		DEL_TEXTURE_BUFFER,
		MAIN_LOOP,
		END,
		UNDEFINED
	};

	struct SGPUTask
	{
		EGPUTaskType TaskType = EGPUTaskType::UNDEFINED;
		SGPUTask() = default;
		virtual ~SGPUTask() = default;
	};

	struct SGPUTaskInitWindow : SGPUTask
	{
		unsigned int Width = 0;
		unsigned int Height = 0;
		SGPUTaskInitWindow(unsigned int vWidth, unsigned int vHeight) : Width(vWidth), Height(vHeight)
		{
			TaskType = EGPUTaskType::INIT_WINDOW;
		}
	};

	struct SGPUTaskGenGeoBuffer : SGPUTask
	{
		unsigned int GeoUID;
		std::shared_ptr<SGeometry> Geometry;
		SGPUTaskGenGeoBuffer(unsigned int vGeoUID, const std::shared_ptr<SGeometry>& vGeometry) : GeoUID(vGeoUID), Geometry(vGeometry)
		{
			TaskType = EGPUTaskType::GEN_GEOMETRY_BUFFER;
		}
	};

	struct SGPUTaskGenTexBuffer : SGPUTask
	{
		unsigned int TexUID;
		std::shared_ptr<STexture> Texture;
		SGPUTaskGenTexBuffer(unsigned int vTexUID, const std::shared_ptr<STexture>& vTexture) : TexUID(vTexUID), Texture(vTexture)
		{
			TaskType = EGPUTaskType::GEN_TEXTURE_BUFFER;
		}
	};

	struct SGPUTaskDelGeoBuffer : SGPUTask
	{
		unsigned int GeoUID;
		SGPUTaskDelGeoBuffer(unsigned int vGeoUID) : GeoUID(vGeoUID)
		{
			TaskType = EGPUTaskType::DEL_GEOMETRY_BUFFER;
		}
	};

	struct SGPUTaskDelTexBuffer : SGPUTask
	{
		unsigned int TexUID;
		SGPUTaskDelTexBuffer(unsigned int vTexUID) : TexUID(vTexUID)
		{
			TaskType = EGPUTaskType::DEL_TEXTURE_BUFFER;
		}
	};

	struct SGPUTaskRender : SGPUTask
	{
		std::shared_ptr<std::vector<unsigned int>> MeshBufferUIDSet;
		SGPUTaskRender(const std::shared_ptr<std::vector<unsigned int>>& vMeshBufferUIDSet) : MeshBufferUIDSet(vMeshBufferUIDSet)
		{
			TaskType = EGPUTaskType::MAIN_LOOP;
		}
	};

	struct SGPUTaskEnd : SGPUTask
	{
		SGPUTaskEnd()
		{
			TaskType = EGPUTaskType::END;
		}
	};
}
