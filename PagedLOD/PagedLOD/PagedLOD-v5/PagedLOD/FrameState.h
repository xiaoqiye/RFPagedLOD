#pragma once
#include "Common.h"

//FIXME:考虑移到common.h中
//结构体定义中是否需要显示定义析构函数
namespace hivePagedLOD
{
	struct IFrameState
	{
		bool EndFlag = false;
		SViewInfo ViewInfo;
		IFrameState() = default;
		virtual ~IFrameState() = default;
	};	
}