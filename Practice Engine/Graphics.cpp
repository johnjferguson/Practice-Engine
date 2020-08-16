#include "Graphics.h"
#include "Assert.h"

#pragma comment(lib, "d3d11.lib" )

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// anti aliasing
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow =  hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = 0;

	// create front / back buffers
	UINT deviceFlag = 0u;
#if _DEBUG
	deviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	ASSERT_HRESULT_INFOQUEUE(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlag,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
	ASSERT_HRESULT_INFOQUEUE(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));
	ASSERT_HRESULT_INFOQUEUE(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));	
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r,g,b };
	ASSERT_INFOQUEUE(pContext->ClearRenderTargetView(pTarget.Get(), color));
}

DxgiInfoManager& Graphics::GetInfoManager()
{
	return infoManager;
}