#include "ImguiManager.h"
#include "ImGui/imgui.h"
#include "Assert.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	context = ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

void ImguiManager::Init()
{
	context = ImGui::CreateContext();
}

void ImguiManager::Destroy()
{
	ImGui::DestroyContext();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
