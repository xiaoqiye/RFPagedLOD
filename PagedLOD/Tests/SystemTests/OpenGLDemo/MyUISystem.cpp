#include "MyUISystem.h"
#include "PagedLODSystem.h"

void CMyUISystem::buildWidget()
{
	ImGui::BeginGroup();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			//if (ImGui::BeginTabItem("Rendering Tile Generator"))
			//{
			//	if (ImGui::RadioButton("no strategy", m_LoadStrategy == hivePagedLOD::ELoadStrategy::DO_NOTHING)) { m_LoadStrategy = hivePagedLOD::ELoadStrategy::DO_NOTHING; } ImGui::SameLine();
			//	if (ImGui::RadioButton("osg strategy", m_LoadStrategy == hivePagedLOD::ELoadStrategy::OSG_LIKE)) { m_LoadStrategy = hivePagedLOD::ELoadStrategy::OSG_LIKE; }
			//	ImGui::EndTabItem();
			//}
			if (ImGui::BeginTabItem("Mesh Buffer Manager"))
			{
				const double MemorySize = static_cast<long double>(hivePagedLOD::CPagedLODSystem::getInstance()->getMemoryBufferManager()->getUsedMemoryCount()) / static_cast<long double>(hivePagedLOD::MEGABYTE);
				ImGui::Text(("Current Memory Size:\t" + std::to_string(MemorySize) + "(MB)").c_str());
				ImGui::SliderInt("limit memory size(MB)", &m_LimitSize, 256, 6144);
				ImGui::SliderFloat("camera speed", &m_CameraSpeed, 1.0f, 1000.0f);

				m_ClearBuffer = false;
				if (ImGui::Button("clear buffers"))
					m_ClearBuffer = true;
				ImGui::EndTabItem();		
			}
			ImGui::EndTabBar();
		}
	ImGui::EndGroup();
}