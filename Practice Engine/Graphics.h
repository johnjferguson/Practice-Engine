#pragma once
#include "LeanWindows.h"
#include <d3d11.h>
#include <wrl.h>
#include "DxgiInfoManager.h"

class Graphics
{
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void DrawTestTriangle();
	void EndFrame();
	void ClearBuffer(float r, float g, float b);
	DxgiInfoManager& GetInfoManager();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr <IDXGISwapChain> pSwap = nullptr;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> pTarget = nullptr;
private:
	int width;
	int height;
	// only initialized in debug mode
#if _DEBUG
	DxgiInfoManager infoManager;
#endif
};
