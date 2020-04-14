#pragma once
#include "Common.h"

namespace hivePagedLOD
{
	struct IFrameState
	{
		bool EndFlag = false;
		SViewInfo ViewInfo;
		virtual ~IFrameState() = default;
	};	
}