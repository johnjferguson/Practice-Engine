#pragma once
#include "LeanWindows.h"
#include <d3d11_4.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include "DxgiInfoManager.h"

class Graphics
{
public:
	Graphics(HWND hWnd, bool windowed, int width, int height);
	~Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	ID3D11Device3* GetDevice();
	ID3D11DeviceContext3* GetContext();
	void DrawTestTriangle();
	void BeginFrame();
	void EndFrame();
	// imgui interface
	void EnableImgui();
	void DisableImgui();
	bool IsImguiEnabled() const;

	void WindowSizeChange(HWND hWnd, int width, int height, bool windowed);
	void CreateWindowSizeDependentResources(HWND hWnd, bool windowed);

	void ClearBuffer(float r, float g, float b);
#if _DEBUG
	DxgiInfoManager& GetInfoManager();
#endif
private:
	// Direct3D objects
	Microsoft::WRL::ComPtr<ID3D11Device3>         m_device = nullptr;
	Microsoft::WRL::ComPtr <IDXGISwapChain3>      m_swap = nullptr;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext3> m_context = nullptr;

	// Direct3D rendering objects
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_target = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_stencil = nullptr;
	D3D11_VIEWPORT                                   m_viewport;

	// Device properties
	D3D_FEATURE_LEVEL	m_featureLevel;
	int                 m_width;
	int                 m_height;

private:
	bool imguiEnabled = true;
};
