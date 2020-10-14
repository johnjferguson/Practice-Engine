#pragma once
#include "LeanWindows.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <dxgidebug.h>

class DxgiInfoManager
{
public:
	static DxgiInfoManager* Instance();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	void Set();
	std::vector<std::wstring> GetMessages() const;
	std::wstring GetConcatenatedMessages() const;
private:
	DxgiInfoManager();
private:
	static DxgiInfoManager infoManager;
	UINT64 next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue = nullptr;
};
