#include "Renderer/Device.h"

#include <guiddef.h>
//#include <Windows.h>

#undef __midl
#include <d3d11.h>
//#include <wrl>
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d11.lib")

#include "Core/Utility/Assert.h"
#include "Core/Containers/FixedArray.h"
#include "Core/Containers/FixedRingBuffer.h"

namespace internal
{
	// Native underlying platform data
	struct DevicePF
	{
		IDXGIFactory1*			factory;
		IDXGIAdapter1*			adapter;
		int8					padding[16];

		IDXGISwapChain*			swapChain;
		ID3D11Device*			device;
		ID3D11DeviceContext*	context;

		ID3D11RenderTargetView* backBufferRTV;
	};
	
	DevicePF* pf = nullptr;
}

using namespace internal;

template <typename ResourceType, typename IDType, size_t capacity>
struct ResourcePool
{
	ResourcePool()
	{
		for (IDType i = capacity; i < 0; i--)
		{
			freeList.Push(i);
		}
	}


	IDType Add(ResourceType resource)
	{
		IDType index = freeList.Pop();
		nativeResources[index] = resource;

		return index;
	}

	void Free(IDType id)
	{
		freeList.Push(id);
	}

private:
	FixedRingBuffer <IDType, capacity>			freeList;
	FixedArray		<ResourceType, capacity>	nativeResources;
};

struct Device::NativeCache
{
	

	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff819065(v=vs.85).aspx
	using Buffers	= ResourcePool<ID3D11Buffer*,				uint8, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT>;
	using Samplers	= ResourcePool<ID3D11SamplerState*,			uint8, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT>;
	using SRVs		= ResourcePool<ID3D11ShaderResourceView*,	uint8, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT>;
	using RTVs		= ResourcePool<ID3D11RenderTargetView*,		uint8, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT>;

	Buffers		buffers;
	Samplers	samplers;
	SRVs		srvs;
	RTVs		rtvs;
};



bool Device::Create(uint32 width, uint32 height, void* handle)
{
	static_assert(sizeof(DevicePF) <= sizeof(pfData), "Internal platform data structure is too big, expand pfData!");
	pf		= reinterpret_cast<DevicePF*>	(pfData);
	cache	= reinterpret_cast<NativeCache*>(new NativeCache);
	memset(cache, 0, sizeof(NativeCache));

	Assert(handle != nullptr, "Invalid window handle for Rendering device!");

	DXGI_SWAP_CHAIN_DESC desc;
	desc.BufferCount						= 1;
	desc.BufferDesc.Width					= width;
	desc.BufferDesc.Height					= height;
	desc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator	= 0;										// This seems OK at 0...
	desc.BufferDesc.RefreshRate.Denominator	= 0;
	desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow						= static_cast<HWND>(handle);
	desc.SampleDesc.Count					= 1;
	desc.SampleDesc.Quality					= 0;
	desc.Windowed							= true;										// Switch later...	
	desc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// Always start in a windowed state
	desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;


	D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_10_0;

#if defined (_DEBUG)
	HRESULT hr = D3D11CreateDeviceAndSwapChain( 0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, &fl, 1, 
												D3D11_SDK_VERSION, &desc, &pf->swapChain, &pf->device, 0, &pf->context);
	if (FAILED(hr))
		return false;
#else
	HRESULT hr = D3D11CreateDeviceAndSwapChain( 0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, &fl, 1, 
												D3D11_SDK_VERSION, &desc, &pf->swapChain, &pf->device, 0, &pf->context);
#endif


	ID3D11Texture2D* backBuffer;
	pf->swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
	pf->device->CreateRenderTargetView(backBuffer, nullptr, &pf->backBufferRTV);

	pf->context->OMSetRenderTargets(1, &pf->backBufferRTV, nullptr);


	D3D11_VIEWPORT viewport;
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= static_cast<float32>(width);
	viewport.Height		= static_cast<float32>(height);
	pf->context->RSSetViewports(1, &viewport);

	return true;
}

BufferID Device::CreateBuffer(Memory* const memory, BufferInfo info)
{	
	D3D11_BUFFER_DESC desc		= { 0 };
	desc.ByteWidth				= static_cast<UINT>(memory->Size);
	desc.CPUAccessFlags			= (memory->Type == MemoryType::HostVisible) ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.Usage					= (memory->Type == MemoryType::DeviceLocal) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;

	if (info.Type == BufferType::VertexBuffer)	 desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (info.Type == BufferType::IndexBuffer)	 desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	if (info.Type == BufferType::ConstantBuffer) desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ID3D11Buffer* buffer;
	if (memory->Data != nullptr)
	{
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem			 = memory->Data;
		srd.SysMemPitch		 = memory->Stride;
		srd.SysMemSlicePitch = 0;

		pf->device->CreateBuffer(&desc, &srd, &buffer);
	}

	pf->device->CreateBuffer(&desc, nullptr, &buffer);

	return cache->buffers.Add(buffer);
}

TextureID Device::CreateTexture(Memory* const memory, TextureInfo info)
{
	union TexDesc
	{
		D3D11_TEXTURE1D_DESC desc1D;
		D3D11_TEXTURE2D_DESC desc2D;
		D3D11_TEXTURE3D_DESC desc3D;
	}desc;
	memset(&desc, 0, sizeof(TexDesc));

	union TexResource
	{
		ID3D11Texture1D* tex1D;
		ID3D11Texture2D* tex2D;
		ID3D11Texture3D* tex3D;
	}res;


	DXGI_FORMAT format;
	if (info.Format == TextureFormat::RGBA) format = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D11_USAGE usage		= (memory->Type == MemoryType::DeviceLocal) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	UINT		cpuAccess	= (memory->Type == MemoryType::HostVisible) ? D3D11_CPU_ACCESS_WRITE : 0;

	
	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem			 = memory->Data;
	srd.SysMemPitch		 = memory->Stride;
	srd.SysMemSlicePitch = 0;

	if (memory->Data != nullptr)
	{
		// Assume the resource is immutable and move it in up front
	}

	
	if (info.Type == TextureType::OneDimensional)
	{
		desc.desc1D.Width			= info.Width;
		desc.desc1D.MipLevels		= info.NumMips;
		desc.desc1D.ArraySize		= 1;
		desc.desc1D.MiscFlags		= 0;
		desc.desc1D.Usage			= usage;
		desc.desc1D.CPUAccessFlags	= cpuAccess;
		
		//pf->device->CreateTexture2D(&desc.desc2D, nullptr, &res.tex2D);
	}

	if (info.Type == TextureType::TwoDimensional)
	{
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;

		//pf->device->CreateTexture1D(&desc.desc1D, nullptr, &res.tex1D);
	}

	if (info.Type == TextureType::ThreeDimensional)
	{
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;
		desc.desc1D.Width = info.Width;

		//pf->device->CreateTexture1D(&desc.desc1D, nullptr, &res.tex1D);
	}


	//uint16 index = cache->buffers.GetSize();
	//pf->device->CreateBuffer(&desc, nullptr, &cache->buffers.At(index));
//
	return 0;
}


void Device::Frame(uint8 test)
{
	float32 f = test / 255.0f;

	float32 color[] = { f, f, 1.0f, 1.0f };
	pf->context->ClearRenderTargetView(pf->backBufferRTV, color);
	pf->swapChain->Present(0, 0);
}