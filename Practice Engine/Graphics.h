#pragma once
#include "LeanWindows.h"
#include <d3d11.h>
#include <wrl.h>
#include "DxgiInfoManager.h"

class Graphics
{
public:
	Graphics(HWND hWnd, int width, int height);
	~Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	void DrawTestTriangle();
	void BeginFrame();
	void EndFrame();
	void EnableImgui();
	void DisableImgui();
	bool IsImguiEnabled() const;
	void ClearBuffer(float r, float g, float b);
#if _DEBUG
	DxgiInfoManager& GetInfoManager();
#endif
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr <IDXGISwapChain> pSwap = nullptr;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> pTarget = nullptr;
private:
	int width;
	int height;
	bool imguiEnabled = true;
	// only initialized in debug mode
#if _DEBUG
	DxgiInfoManager infoManager;
#endif
};
