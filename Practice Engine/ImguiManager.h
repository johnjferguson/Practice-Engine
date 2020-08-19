#pragma once
#include "ImGui/imgui.h"

class ImguiManager
{
public:
	ImguiManager();
	void Init();
	void Destroy();
	~ImguiManager();
	ImguiManager(const ImguiManager&) = delete;
	ImguiManager& operator=(const ImguiManager&) = delete;
private:
	ImGuiContext* context = nullptr;
};
