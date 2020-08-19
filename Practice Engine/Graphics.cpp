#include "Graphics.h"
#include "Assert.h"
#include <d3dcompiler.h>
#include "ImGui/examples/imgui_impl_dx11.h"
#include "ImGui/examples//imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib" )
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd, int width, int height)
	:
	width(width),
	height(height)
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

	// init imgui directx11 impl
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

ID3D11Device* Graphics::GetDevice()
{
	return pDevice.Get();
}

ID3D11DeviceContext* Graphics::GetContext()
{
	return pContext.Get();
}

void Graphics::DrawTestTriangle()
{
	struct Vertex
	{
		float x;
		float y;
	};

	const Vertex vertices[] =
	{
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f}
	};


	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = vertices;
	ASSERT_HRESULT_INFOQUEUE( pDevice->CreateBuffer(&bd, &srd, &pVertexBuffer));
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	ASSERT_INFOQUEUE(pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));

	// vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	ASSERT_HRESULT_INFOQUEUE(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	ASSERT_HRESULT_INFOQUEUE(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	ASSERT_INFOQUEUE(pContext->VSSetShader(pVertexShader.Get(), nullptr, 0));

	// input layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	ASSERT_HRESULT_INFOQUEUE(pDevice->CreateInputLayout(ied, 1u, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));

	pContext->IASetInputLayout(pInputLayout.Get());

	//pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;

	ASSERT_HRESULT_INFOQUEUE(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	ASSERT_HRESULT_INFOQUEUE(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	ASSERT_INFOQUEUE(pContext->PSSetShader(pPixelShader.Get(),nullptr, 0));

	// set topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	// configure viewport
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ASSERT_INFOQUEUE(pTarget->GetDesc(&rtvd));

	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	ASSERT_INFOQUEUE(pContext->Draw(std::size( vertices), 0u));
}

void Graphics::BeginFrame()
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void Graphics::EndFrame()
{
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	ASSERT_HRESULT_INFOQUEUE(pSwap->Present(1u, 0u));
}

void Graphics::EnableImgui()
{
	imguiEnabled = true;
}

void Graphics::DisableImgui()
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const
{
	return imguiEnabled;
}

void Graphics::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r,g,b };
	ASSERT_INFOQUEUE(pContext->ClearRenderTargetView(pTarget.Get(), color));
}

#if _DEBUG
DxgiInfoManager& Graphics::GetInfoManager()
{
	return infoManager;
}
#endif
