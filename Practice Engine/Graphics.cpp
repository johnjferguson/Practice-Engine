#include "Graphics.h"
#include "Assert.h"
#include <d3dcompiler.h>
#include <wincodec.h>
#include <random>
#include <DirectXMath.h>
#include "ImGui/examples/imgui_impl_dx11.h"
#include "ImGui/examples//imgui_impl_win32.h"
#include "Geometry.h"
#include "DirectXTK/WICTextureLoader.h"
#include "Assimp/code/Obj/ObjFileData.h"
#include "Assimp/code/Obj/ObjFileImporter.h"

#pragma comment(lib, "d3d11.lib" )
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


Graphics::Graphics(HWND hWnd, bool windowed, int width, int height)
	:
	m_width(width),
	m_height(height)
{
	// This flag is required for compatibility with Direct2D
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	wrl::ComPtr<ID3D11Device> device;
	wrl::ComPtr<ID3D11DeviceContext> context;

	ASSERT_HR(D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		&device,					// Returns the Direct3D device created.
		&m_featureLevel,			// Returns feature level of device created.
		&context					// Returns the device immediate context.
	));

	// store pointers in member functions since they required createDevice required device and not device3
	ASSERT_HR(device.As(&m_device));
	ASSERT_HR(context.As(&m_context));
	
	CreateWindowSizeDependentResources(hWnd, windowed);

	// init imgui directx11 impl
	ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

ID3D11Device3* Graphics::GetDevice()
{
	return m_device.Get();
}

ID3D11DeviceContext3* Graphics::GetContext()
{
	return m_context.Get();
}

void Graphics::DrawTestTriangle()
{
	// testing object file loading
	std::vector<char> buff;
	std::string path = "Models\\spider.mtl";
	Assimp::ObjFile::Model model;
	Assimp::ObjFileImporter importer;
	//Assimp::ObjFileMtlImporter(buff, path, &model);


	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	struct CBuff
	{
		dx::XMMATRIX transform;
		dx::XMMATRIX projection;
	};


	// temp --------------------------------------
	ImGui::SliderFloat("pitch", &m_pitch, -10, 10);
	ImGui::SliderFloat("roll", &m_roll, -10, 10);
	ImGui::SliderFloat("yaw", &m_yaw, -10, 10);

	ImGui::SliderFloat("z", &m_z, 0, 20);
	// ^------------------------------------------

	CBuff cBuff = { dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) * dx::XMMatrixTranslation(0.0f, 0.0f, m_z), dx::XMMatrixPerspectiveFovLH(1.5707, float(m_width) / float(m_height), 1.0f, 100.0f )};

	struct Vertex
	{
		float x;
		float y;
		float z;
		float u;
		float v;
	};

	wrl::ComPtr<ID3D11Resource> pResource;
	wrl::ComPtr<ID3D11ShaderResourceView> pResourceView;
	wrl::ComPtr<ID3D11SamplerState> pSamplerState;

	D3D11_SAMPLER_DESC ssd = {};
	ssd.Filter = D3D11_FILTER_ANISOTROPIC;
	ssd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	ssd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ssd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	

	m_device->CreateSamplerState(&ssd, &pSamplerState);
	m_context->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());

	ASSERT_HR_INFO(dx::CreateWICTextureFromFile(m_device.Get(), L"Images//grass.jpg", &pResource, &pResourceView));
	m_context->PSSetShaderResources(0, 1, pResourceView.GetAddressOf());

	IndexedTriangleList<Vertex> cube = MakeCube<Vertex>(1.0f, 1.0f, 1.0f);

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = cube.vertices.size() * sizeof(Vertex);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = cube.vertices.data();
	ASSERT_HR_INFO( m_device->CreateBuffer(&bd, &srd, &pVertexBuffer));
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	ASSERT_INFO(m_context->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));

	// vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	ASSERT_HR_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	ASSERT_HR_INFO(m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	ASSERT_INFO(m_context->VSSetShader(pVertexShader.Get(), nullptr, 0));

	// input layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ASSERT_HR_INFO(m_device->CreateInputLayout(ied, 2u, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));

	m_context->IASetInputLayout(pInputLayout.Get());

	//pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;

	ASSERT_HR_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	ASSERT_HR_INFO(m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	ASSERT_INFO(m_context->PSSetShader(pPixelShader.Get(),nullptr, 0));

	// set topology
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// bind render target
	m_context->OMSetRenderTargets(1u, m_target.GetAddressOf(), nullptr);

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.ByteWidth = sizeof(CBuff);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.StructureByteStride = sizeof(CBuff);
	D3D11_SUBRESOURCE_DATA csrd = {};
	csrd.pSysMem = &cBuff;

	ASSERT_HR_INFO(m_device->CreateBuffer(&cbd, &csrd, &pConstantBuffer));
	m_context->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());

	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = cube.indices.size() * sizeof(unsigned short);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isrd = {};
	isrd.pSysMem = cube.indices.data();

	ASSERT_HR_INFO(m_device->CreateBuffer(&ibd, &isrd, &pIndexBuffer));
	m_context->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// configure viewport
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ASSERT_INFO(m_target->GetDesc(&rtvd));

	m_context->RSSetViewports(1u, &m_viewport);

	ASSERT_INFO(m_context->DrawIndexed(cube.indices.size(), 0, 0));
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

	ASSERT_HR_INFO(m_swap->Present(0u, 0u));
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

void Graphics::WindowSizeChange(HWND hWnd, int width, int height, bool windowed)
{
	m_width = width;
	m_height = height;

	CreateWindowSizeDependentResources(hWnd, windowed);
}

void Graphics::CreateWindowSizeDependentResources(HWND hWnd, bool windowed)
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_target = nullptr;
	m_stencil = nullptr;
	m_context->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	if (m_swap != nullptr)
	{
		ASSERT_HR(m_swap->ResizeBuffers(
			2, // Double-buffered swap chain.
			m_width,
			m_height,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		));
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd = {};
		DXGI_RATIONAL rational = {};

		sd.Width = m_width;
		sd.Height = m_height;
		sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.Stereo = false;
		sd.SampleDesc.Count = 1;												  // anti aliasing
		sd.SampleDesc.Quality = 0;                                               
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;                            // All Microsoft Store apps must use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
		sd.Flags = 0;
		sd.Scaling = DXGI_SCALING_NONE;
		sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		rational.Denominator = 0;
		rational.Numerator = 0;
		fsd.RefreshRate.Numerator = 0;
		fsd.RefreshRate.Denominator = 0;
		fsd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fsd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fsd.Windowed = windowed;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		wrl::ComPtr<IDXGIDevice3> dxgiDevice;
		ASSERT_HR(
			m_device.As(&dxgiDevice)
		);

		wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
		ASSERT_HR(
			dxgiDevice->GetAdapter(&dxgiAdapter)
		);

		wrl::ComPtr<IDXGIFactory4> dxgiFactory;
		ASSERT_HR(
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
		);

		wrl::ComPtr<IDXGISwapChain1> swapChain;
		ASSERT_HR(
			dxgiFactory->CreateSwapChainForHwnd(
				m_device.Get(),
				hWnd,
				&sd,
				&fsd,
				nullptr,
				&swapChain
			)
		);

		ASSERT_HR(
			swapChain.As(&m_swap)
		);
		
		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		ASSERT_HR(
			dxgiDevice->SetMaximumFrameLatency(1)
		);
	}

	wrl::ComPtr<ID3D11Texture2D1> backBuffer;
	ASSERT_HR(m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D1), &backBuffer));

	ASSERT_HR(
		m_device->CreateRenderTargetView(
			backBuffer.Get(),
			nullptr,
			&m_target
		)
	);

	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		m_width,
		m_height,
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	wrl::ComPtr<ID3D11Texture2D1> depthStencil;
	ASSERT_HR(
		m_device->CreateTexture2D1(
			&depthStencilDesc,
			nullptr,
			&depthStencil
		)
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	ASSERT_HR(
		m_device->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			&m_stencil
		)
	);

	// Set the 3D rendering viewport to target the entire window.
	m_viewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		float(m_width),
		float(m_height)
	);

	m_context->RSSetViewports(1, &m_viewport);
}

void Graphics::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r,g,b };
	ASSERT_INFO(m_context->ClearRenderTargetView(m_target.Get(), color));
}

