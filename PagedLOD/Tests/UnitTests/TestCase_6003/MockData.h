#pragma once
#include "PagedLOD/Tile.h"
#include "PagedLOD/Common.h"
#include <GLM/glm.hpp>

#include <vector>

namespace MockData
{
	const std::vector<std::shared_ptr<SGPUTask>> getGPUBufferTaskSet()
	{
		std::vector<std::shared_ptr<SGPUTask>> GPUTaskSet;
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskDelGeoBuffer>("0.bin"));
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskDelTexBuffer>("0.bint"));
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskGenGeoBuffer>("1.bin"));
		GPUTaskSet.emplace_back(std::make_shared<SGPUTaskGenTexBuffer>("1.bint"));

		return GPUTaskSet;
	}


	const std::vector<std::shared_ptr<SGPUTask>> getGPURenderTaskSet()
	{
		std::vector<std::shared_ptr<SGPUTask>> GPUTaskSet;
		std::shared_ptr<SGPUTaskRender> TaskRender;
		//todo: mock geo handle and tex id

		GPUTaskSet.emplace_back(TaskRender);

		return GPUTaskSet;
	}
}