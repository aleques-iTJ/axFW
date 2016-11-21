#pragma once
#include "Renderer/DX11/RendererDX11.h"
#include "Renderer/DX11/TextureDX11.h"

//----------------------------------------------------------------

RendererDX11::RendererDX11()
{

}

RendererDX11::~RendererDX11()
{
	Destroy();
}

//----------------------------------------------------------------

bool RendererDX11::Create(WindowHandle handle, u32 width, u32 height)
{
	// Just try to grab the foreground window if the window handle is null
	if (handle == nullptr)
		windowHandle = GetForegroundWindow();
	else
		windowHandle = handle;

	// The swap chain isn't wrapped
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferCount							= 1;
	scd.BufferDesc.Width					= width;
	scd.BufferDesc.Height					= height;
	scd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator	= 60;										// This is WRONG, should be properly enumerated!
	scd.BufferDesc.RefreshRate.Denominator	= 1;
	scd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow						= windowHandle;
	scd.SampleDesc.Count					= 1;
	scd.SampleDesc.Quality					= 0;
	scd.Windowed							= true;										// Switch later...	
	scd.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// Always start in a windowed state
	scd.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_10_0;
#ifdef _DEBUG	
	HRESULT hr = D3D11CreateDeviceAndSwapChain( 0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, 
												&fl, 1, D3D11_SDK_VERSION, &scd, &swapChain, &device, 0, &context);
#else
	HRESULT hr = D3D11CreateDeviceAndSwapChain( 0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_SINGLETHREADED, 
												&fl, 1, D3D11_SDK_VERSION, &scd, &swapChain, &device, 0, &context);
#endif

	if (hr == S_OK) // We're okay!
	{
		// Some setup of our own
		backBuffer	= new TextureDX11;
		depthBuffer = new TextureDX11;
		memset(&frameInfo, 0, sizeof(FrameInfo));

		// Start getting set up to actually... do D3D things
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer->tex);
		device->CreateRenderTargetView(backBuffer->tex, 0, &backBuffer->rtv);

		backBuffer->SetDefaults();
		depthBuffer->SetDefaults();
		depthBuffer->Width	= width;
		depthBuffer->Height = height;
		depthBuffer->Format = DataFormat::DepthStencil;
		depthBuffer->BindAs = BindFlags::DepthStencil;
		backBuffer->BindAs	= BindFlags::RenderTarget;

		// GetBuffer() has already done the rest internally for the backbuffer
		// This is why nothing is really set for its texture
		this->CreateTexture(*depthBuffer);

		// Just cheat this time and bind it immediately rather than deferring it to PreRender()
		ID3D11RenderTargetView* temp = backBuffer->rtv.GetInterface();
		this->context->OMSetRenderTargets(1, &temp, depthBuffer->dsv);

		Viewport vp;
		vp.Width = static_cast<f32>(width);
		vp.Height = static_cast<f32>(height);
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		this->SetViewport(vp);

		// We've probably managed to survive if we're this far, so call it a success
		return true;
	}

	return false;
}

bool RendererDX11::Destroy()
{
	// delete depthBuffer;
	// delete backBuffer;

	// The DX11 resources should release themselves ... HOPEFULLY
	// Don't be surprised if something leaks ATM!
	return true;
}

//----------------------------------------------------------------

void RendererDX11::PreRender()
{
	// Obliterate the per frame stats, time to start anew
	memset(&frameInfo, 0, sizeof(FrameInfo));
}

void RendererDX11::PostRender() const
{

}

void RendererDX11::Clear() const
{
	const f32 color[4] = { 0.0f, 0.1f, 0.0f, 0.0f };

	context->ClearRenderTargetView(backBuffer->rtv, color);
	context->ClearDepthStencilView(depthBuffer->dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RendererDX11::Present() const
{
	swapChain->Present(0, 0);
}

//----------------------------------------------------------------

ResourceID RendererDX11::CreateBuffer(BufferDX11 & buffer)
{
	return ResourceID();
}

ResourceID RendererDX11::CreateTexture(TextureDX11& texture)
{
	D3D11_TEXTURE2D_DESC desc;

	// Do some ugly translation...
	desc.ArraySize		= 1;
	desc.MipLevels		= texture.MipMapCount;
	desc.MiscFlags		= 0;
	desc.SampleDesc		= { 1, 0 };
	desc.CPUAccessFlags = static_cast<UINT>(texture.CPUAccessType);
	desc.Format			= static_cast<DXGI_FORMAT>(texture.Format);
	desc.Height			= texture.Height;
	desc.Width			= texture.Width;
	desc.Usage			= static_cast<D3D11_USAGE>(texture.Usage);
	desc.BindFlags		= static_cast<UINT>(texture.BindAs);

	// Use any existing sub resources as initial data
	u32 numSubResources = texture.Subresources.GetSize();
	if (numSubResources > 0)
	{
		for (u32 i = 0; i < numSubResources; i++)
		{
			D3D11_SUBRESOURCE_DATA* srd = new D3D11_SUBRESOURCE_DATA[numSubResources];
			srd[i].pSysMem			= texture.Subresources[i].Data;
			srd[i].SysMemPitch		= texture.Subresources[i].Stride;
			srd[i].SysMemSlicePitch = 0;

			device->CreateTexture2D(&desc, srd, &texture.tex);
			delete srd;
		}
	}

	else
		device->CreateTexture2D(&desc, nullptr, &texture.tex);

	// Create views 
	if (desc.BindFlags & static_cast<UINT>(BindFlags::ShaderResource))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format						= desc.Format;
		srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels			= desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip	= 0;

		device->CreateShaderResourceView(texture.tex, &srvDesc, &texture.srv);
	}

	if (desc.BindFlags & static_cast<UINT>(BindFlags::RenderTarget))
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format				= desc.Format;
		rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice	= 0;

		device->CreateRenderTargetView(texture.tex, &rtvDesc, &texture.rtv);
	}

	if (desc.BindFlags & static_cast<UINT>(BindFlags::DepthStencil))
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format				= desc.Format;
		dsvDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice	= 0;
		dsvDesc.Flags				= 0;

		device->CreateDepthStencilView(texture.tex, &dsvDesc, &texture.dsv);
	}

	managedTextures.PushBack(&texture);
	texture.resourceID = managedTextures.GetSize();

	return texture.resourceID;
}

ResourceID RendererDX11::CreateShader(ShaderDX11& shader)
{
	return ResourceID();
}