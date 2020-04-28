#include "MyUISystem.h"
#include "PagedLODSystem.h"

void CMyUISystem::buildWidget()
{
	//ImGui::BeginGroup();
	//if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
	//{
	//	if (ImGui::BeginTabItem("Camera"))
	//	{
	//		ImGui::SliderFloat("camera speed", &m_CameraSpeed, 1.0f, 1000.0f);
	//		ImGui::EndTabItem();
	//	}
	//	if (ImGui::BeginTabItem("Mesh Buffer Manager"))
	//	{
	//		const double MemorySize = static_cast<long double>(hivePagedLOD::CPagedLODSystem::getInstance()->getMemoryBufferManager()->getUsedMemoryCount()) / static_cast<long double>(hivePagedLOD::MEGABYTE);
	//		ImGui::Text(("current memory size:\t" + std::to_string(MemorySize) + "(MB)").c_str());
	//		ImGui::SliderInt("limit memory size(MB)", &m_BufferLimitSizeProxy, 1, 2048);
	//		m_BufferLimitSize = m_BufferLimitSizeProxy * hivePagedLOD::MEGABYTE;
	//		ImGui::EndTabItem();
	//	}
	//	if (ImGui::BeginTabItem("Rendering Tile Generator"))
	//	{
	//		if (m_LoadStrategy != hivePagedLOD::ELoadStrategy::UNDEFINED)
	//		{
	//			ImGui::SliderInt("limit memory load size(KB/Frame)", &m_LoadLimitSizePerFrameProxy, 1, 2048);
	//			ImGui::SliderInt("load parent max level", &m_LoadParentMaxLevel, 1, 10);
	//			m_LoadLimitSizePerFrame = m_LoadLimitSizePerFrameProxy * hivePagedLOD::KILOBYTE;
	//		}
	//		ImGui::EndTabItem();
	//	}
	//	ImGui::EndTabBar();
	//}
	//ImGui::EndGroup();
}