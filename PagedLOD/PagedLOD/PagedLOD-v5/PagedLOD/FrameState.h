#pragma once
#include "Common.h"

//FIXME:�����Ƶ�common.h��
//�ṹ�嶨�����Ƿ���Ҫ��ʾ���������������ṹ������Ҳ����
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