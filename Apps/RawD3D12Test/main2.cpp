#include "bnr.h"

#if defined (BNR_DEBUG)
	#if !defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Debug-x86.lib")
	#elif defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Debug-x64.lib")
		//#pragma comment(lib, "Renderer-Debug-x64.lib")
	#endif
#else
	#if !defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Release-x86.lib")
	#elif defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Release-x64.lib")
	#endif
#endif

//==============================================================================================
// dkg;lnansdlkm
//==============================================================================================

class TestApp
{
public:
	TestApp ();
	~TestApp();

	void Run();

private:
	Window*		window;
	// Renderer*	renderer;
};


TestApp::TestApp()
{
	window		= new Window;
	// renderer	= new Renderer;
}

TestApp::~TestApp()
{
	// Reverse order from init
	// if (renderer)	delete renderer;
	if (window)			delete window;
}


//==============================================================================================
// Barfo
//==============================================================================================

struct PCTVertex
{
	float32 x, y, z;	// 12 bytes, the literal position in pixels
	float32 r, g, b;	// 24 bytes, color to be modulated against
	float32 u, v;		// 32 bytes, tex coordinate
};

// 32 bit integer color
struct ColorInt
{
	union
	{
		struct
		{
			uint8 Red;
			uint8 Green;
			uint8 Blue;
			uint8 Alpha;
		};

		uint8  Array[4];
		uint32 ColorARGB;
	};
};

struct Particle
{
	float32 X;
	float32 Y;
	float32 LastX;
	float32 LastY;
};


constexpr uint32 WIDTH			= 512;
constexpr uint32 HEIGHT			= 512;
constexpr uint32 NUM_PARTICLES	= 4096;

#include "Helpers.h"

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

//----------------------------------------------------------------------------------------------

void UpdateWindowFrametimeInfo(Window& window, Timer& timer)
{
	timer.Update();
	f64 hz = timer.GetHzAvg();
	f64 ms = timer.GetMsAvg();
	f64 ac = timer.GetAccumulator();

	if (ac >= 1000.0f)
	{
		char str[128];
		sprintf(str, "bnrFW - FPS:%f; MS:%f", hz, ms);
		SetWindowText(window.GetHandle(), str);

		timer.ResetAccumuator();
	}
}

// Some old pixel / line functions ripped from an old project
void SetPixel(u32 x, u32 y, const ColorInt& col, void* ptr)
{
	// http://stereopsis.com/doubleblend.html
	// 32 bit integer alpha blending

	// blended = ((alpha * (src - dst)) / 255) + dst
	u32* dst = reinterpret_cast<u32*>(ptr) + ((y * WIDTH) + x);

	// Try to early out if possible, if it's totally opaque or transparent
	if (col.Alpha == 0xFF)	{ *dst = col.ColorARGB;	return; }
	if (col.Alpha == 0x00)	{ *dst = 0x00000000;	return; }
	else
	{
		// Part of the trick is the sort of SIMD-esque manipulation		
		// We need to prevent the overflow ourselves, which is why 
		// it works on 2 channels per u32, so split up the channels
		u32 dstAG	= *dst		& 0xFF00FF00;
		u32 dstRB	= *dst		& 0x00FF00FF;
		u32 srcAG	= col.ColorARGB	& 0xFF00FF00;
		u32 srcRB	= col.ColorARGB	& 0x00FF00FF;

		// src - dst
		dstAG		= srcAG - dstAG;
		dstRB		= srcRB - dstRB;

		// alpha * (src - dst)		
		dstAG		*= col.Alpha;
		dstRB		*= col.Alpha;

		// (alpha * (src - dst)) / 255
		// Cheat by dividing by 256 (for the fast bit shift) instead of 255
		dstAG		>>= 8;
		dstRB		>>= 8;

		// ((alpha * (src - dst)) / 255) + dst
		// Mask off the garbage from overflow and combine the channels again
		dstAG		= dstAG & 0xFF00FF00;
		dstRB		= dstRB & 0x00FF00FF;
		*dst		= ((dstAG | dstRB) + *dst);
	}
}

void SetPixelSafe(u32 x, u32 y, const ColorInt& col, void* ptr)
{
	// Check the texture bounds before writing
	if ((x | y) >= 0 && x < WIDTH  && y < HEIGHT)
	{
		SetPixel(x, y, col, ptr);
	}
}

// Naive DDA line
void DrawLine(u32 x1, u32 y1, u32 x2, u32 y2, const ColorInt& col, void* ptr)
{	
	f32 startX = static_cast<f32>(x1);
	f32 startY = static_cast<f32>(y1);	
	
	// The length, can be negative here depending on the line orientation (see: upcoming abs())
	f32 deltaX = static_cast<f32>(static_cast<s32>(x2 - x1));	
	f32 deltaY = static_cast<f32>(static_cast<s32>(y2 - y1));

	// How many pixels we need to move to cover that distance
	u32 steps;
	abs(deltaX) > abs(deltaY) ?	steps = Round(abs(deltaX)) :
					steps = Round(abs(deltaY));

	// How much we need to increment in that given direction
	f32 stepSizeX = deltaX / steps;
	f32 stepSizeY = deltaY / steps;

	foreach(i, steps)
	{
		// We don't really need to clip the line against the texture boundary
		// since we just play it safe and don't try to draw the pixel anyway
		SetPixelSafe(Round(startX), Round(startY), col, ptr);
		startX += stepSizeX;
		startY += stepSizeY;
	}
}

//----------------------------------------------------------------------------------------------

class D3D12Test
{
public:	
	// Helper so this is slightly less obnoxious to type
	void ResourceBarrier(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
	{
		D3D12_RESOURCE_BARRIER desc;
		memset(&desc, 0, sizeof(D3D12_RESOURCE_BARRIER));

		desc.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Transition.pResource	= res;
		desc.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc.Transition.StateBefore	= before;
		desc.Transition.StateAfter	= after;
		desc.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commandList->ResourceBarrier(1, &desc);
	}


	// Take a guess
	void InitSwapChain(WindowHandle handle)
	{
		if (handle == nullptr)
			return; // Invalid window handle
		this->handle = handle;


		// Fall back to a warp device when in doubt
		IDXGIFactory4*	dxgiFactory = nullptr;
		//IDXGIAdapter3*	adapater;
		CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
		//dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapater));*/
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&d3dDevice));


		// DX11's immediate contexts are gone, all command lists all day erry day
		D3D12_COMMAND_QUEUE_DESC cqDesc;
		memset(&cqDesc, 0, sizeof(D3D12_COMMAND_QUEUE_DESC));
		cqDesc.Type	= D3D12_COMMAND_LIST_TYPE_DIRECT;
		cqDesc.Flags	= D3D12_COMMAND_QUEUE_FLAG_NONE;
		d3dDevice->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&commandQueue));
		d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));


		// Start setting up DXGI and the swap chain... looks just like D3D11
		DXGI_SWAP_CHAIN_DESC scd;
		scd.BufferCount							= 2;
		scd.BufferDesc.Width					= WIDTH;
		scd.BufferDesc.Height					= HEIGHT;
		scd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator	= 0;							// Seems OK to just use 0
		scd.BufferDesc.RefreshRate.Denominator	= 0;
		scd.BufferDesc.Scaling					= DXGI_MODE_SCALING_CENTERED;
		scd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow						= handle;
		scd.SampleDesc.Count					= 1;
		scd.SampleDesc.Quality					= 0;
		scd.Windowed							= true;								// Always start in a windowed state
		scd.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		scd.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// Must use FLIP swap effect in DX12

		
		// Apparently it takes the queue instead of the device??
		dxgiFactory->CreateSwapChain(commandQueue.Get(), &scd, reinterpret_cast<IDXGISwapChain**>(dxgiSwapChain.GetAddressOf()));
		dxgiFactory->Release();

		
		// RTV descriptor heap for the back buffers
		D3D12_DESCRIPTOR_HEAP_DESC dhDesc;
		memset(&dhDesc, 0, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		dhDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		dhDesc.NumDescriptors	= 2;
		d3dDevice->CreateDescriptorHeap(&dhDesc, IID_PPV_ARGS(&descHeapRTV));


		// Get the render targets for the back buffers	
		foreach(i, 2)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE	descHandle	= descHeapRTV->GetCPUDescriptorHandleForHeapStart();
							descHandle.ptr += i * d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&swapRTV[i]));
			d3dDevice->CreateRenderTargetView(swapRTV[i].Get(), nullptr, descHandle);
		}
	}

	void InitResources()
	{
		// This fence will be paramount later...
		d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		fenceEvent = CreateEvent(nullptr, 0, 0, nullptr);

		
		// One the projection matrix cbuffer and texture srv, another for the texture sampler
		D3D12_DESCRIPTOR_HEAP_DESC dhDesc[2];
		foreach(i, 2)
		{
			dhDesc[i].NumDescriptors = 16;
			dhDesc[i].Flags		 = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			dhDesc[i].NodeMask	 = 0;
		}
		
		dhDesc[0].Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		dhDesc[1].Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		d3dDevice->CreateDescriptorHeap(&dhDesc[0], IID_PPV_ARGS(&descHeapCBVSRV));
		d3dDevice->CreateDescriptorHeap(&dhDesc[1], IID_PPV_ARGS(&descHeapSamplers));

		CD3D12_DESCRIPTOR_RANGE descRangeCBVSRV[2];
		CD3D12_DESCRIPTOR_RANGE descRangeSamplers;
		descRangeCBVSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		descRangeCBVSRV[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		
		descRangeSamplers.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

		CD3D12_ROOT_PARAMETER rootParam[2];
		rootParam[0].InitAsDescriptorTable(2, descRangeCBVSRV);
		rootParam[1].InitAsDescriptorTable(1, &descRangeSamplers);

		ID3D10Blob*			signature = nullptr;
		D3D12_ROOT_SIGNATURE_DESC	signatureDesc;
		memset(&signatureDesc, 0, sizeof(D3D12_ROOT_SIGNATURE_DESC));
		signatureDesc.NumParameters	= 2;
		signatureDesc.pParameters	= rootParam;		
		signatureDesc.Flags		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // Is this enum's name long enough?

		D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
		d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		signature->Release();

		// The input layout for the quad
		D3D12_INPUT_ELEMENT_DESC elementDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,				 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0	},
			{ "COLOR",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0	},
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0	}
		};

		D3D12_INPUT_LAYOUT_DESC inputLayout = { elementDesc, 3 };


		// Load and compile the sprite shader
		String shaderPath = System::GetExeDirectory();
		shaderPath.Append("BasicSprite_PCT.txt");
		File* shader = FileSystem::OpenFile(shaderPath, FileAccess::Read);
		FileSystem::ReadFile(shader);

		ID3D10Blob* vsBuffer = nullptr;
		ID3D10Blob* psBuffer = nullptr;
		D3DCompile(shader->GetDataPtr(), shader->GetSize(), nullptr, nullptr, nullptr, "vsMain", "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsBuffer, nullptr);
		D3DCompile(shader->GetDataPtr(), shader->GetSize(), nullptr, nullptr, nullptr, "psMain", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psBuffer, nullptr);


		// Set the pipeline state...
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		memset(&psoDesc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc.VS.pShaderBytecode		= vsBuffer->GetBufferPointer();
		psoDesc.VS.BytecodeLength		= vsBuffer->GetBufferSize();
		psoDesc.PS.pShaderBytecode		= psBuffer->GetBufferPointer();
		psoDesc.PS.BytecodeLength		= psBuffer->GetBufferSize();
		psoDesc.SampleDesc.Count		= 1;
		psoDesc.SampleMask			= UINT_MAX;
		psoDesc.InputLayout.pInputElementDescs	= elementDesc;
		psoDesc.InputLayout.NumElements		= 3;
		psoDesc.pRootSignature			= rootSignature.Get();
		psoDesc.NumRenderTargets		= 1;
		psoDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.PrimitiveTopologyType		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RasterizerState			= CD3D12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState		= CD3D12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.BlendState			= CD3D12_BLEND_DESC(D3D12_DEFAULT);			
		d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
		vsBuffer->Release();
		psBuffer->Release();


		// Set the viewport and scissor
		viewport.MaxDepth			= 1.0f;
		viewport.MinDepth			= 0.0f;
		viewport.TopLeftX			= 0.0f;
		viewport.TopLeftY			= 0.0f;
		viewport.Width				= static_cast<FLOAT>(WIDTH);
		viewport.Height				= static_cast<FLOAT>(HEIGHT);

		// Why did they randomly change case here?
		scissor.top				= 0;
		scissor.left				= 0;
		scissor.right				= WIDTH;
		scissor.bottom				= HEIGHT;
	}


//----------------------------------------------------------------------------------------------

	void InitData()
	{
		// Start setting up and loading the buffers
		// The quad
		PCTVertex verts[4];
		verts[0]	= { 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f	}; // Top left
		verts[1]	= { WIDTH, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f	}; // Top Right
		verts[2]	= { WIDTH, HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f	}; // Bottom right
		verts[3]	= { 0.0f, HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f	}; // Bottom left		
		u16 indicies[6] = { 0, 1, 2, 2, 3, 0 };					   // TL, TR, BR, BR, BL, TL

		// Vertex and index buffer - they both exist in the same buffer!
		d3dDevice->CreateCommittedResource(	&CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
							&CD3D12_RESOURCE_DESC::Buffer(sizeof(verts) + sizeof(indicies)),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vbibData));

		// Copy in the vertex and index data
		char* ptr = nullptr;

		vbibData->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
		memcpy(ptr, verts, sizeof(verts));				// VB first
		memcpy(ptr + sizeof(verts), indicies, sizeof(indicies));	// IB next
		vbibData->Unmap(0, nullptr);
		vbView.BufferLocation		= vbibData->GetGPUVirtualAddress();
		vbView.StrideInBytes		= sizeof(PCTVertex);
		vbView.SizeInBytes		= sizeof(verts);
		ibView.BufferLocation		= vbibData->GetGPUVirtualAddress() + sizeof(verts);
		ibView.Format			= DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes		= sizeof(indicies);

		// Constant buffer
		d3dDevice->CreateCommittedResource(	&CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
							&CD3D12_RESOURCE_DESC::Buffer(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&cbData));

		// CBV, then SRV
		D3D12_CPU_DESCRIPTOR_HANDLE descHeapCBVSRVHandle = descHeapCBVSRV->GetCPUDescriptorHandleForHeapStart();

		// Copy in the projection matrix data
		Matrix proj	= Matrix::Ortho2D(static_cast<FLOAT>(WIDTH), static_cast<FLOAT>(HEIGHT), 0.1f, 100.0f);
		cbData->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
		memcpy(ptr, &proj, sizeof(Matrix));
		cbData->Unmap(0, nullptr);
		cbView.BufferLocation		= cbData->GetGPUVirtualAddress();
		cbView.SizeInBytes		= D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		d3dDevice->CreateConstantBufferView(&cbView, descHeapCBVSRVHandle); // CB at the start

		// CPU side texture - this is the working texture that is copied to the one in the default heap later
		d3dDevice->CreateCommittedResource(	&CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
							&CD3D12_RESOURCE_DESC::Buffer((WIDTH * HEIGHT) * 4),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&texDataCPU));

		// GPU memory texture - state starts as generic read and transitions for the copy later
		d3dDevice->CreateCommittedResource(	&CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
							&CD3D12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, WIDTH, HEIGHT, 1, 1),
							D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&texDataGPU));

		texDataCPU->Map(0, nullptr, &particlesWorkingTexture);


		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension		= D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping	= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels	= 1;

		descHeapCBVSRVHandle.ptr	+= d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		d3dDevice->CreateShaderResourceView(texDataGPU.Get(), &srvDesc, descHeapCBVSRVHandle);

		D3D12_SAMPLER_DESC samplerDesc;
		memset(&samplerDesc, 0, sizeof(D3D12_SAMPLER_DESC));
		samplerDesc.Filter		= D3D12_FILTER_ANISOTROPIC;
		samplerDesc.AddressU		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.MinLOD		= -FLT_MAX;
		samplerDesc.MaxLOD		= FLT_MAX;
		samplerDesc.ComparisonFunc	= D3D12_COMPARISON_FUNC_NEVER;
		d3dDevice->CreateSampler(&samplerDesc, descHeapSamplers->GetCPUDescriptorHandleForHeapStart());
	}

	void Draw()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE	descHandle	= descHeapRTV->GetCPUDescriptorHandleForHeapStart();
		UINT				backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
						descHandle.ptr += backBufferIndex * d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		f32				color[]		= { 0.0f, 0.3f, 0.0f, 0.0f };		
		
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(handle, &point);

		ResourceBarrier(texDataGPU.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
		AttractAndDrawParticles(Vector2<u32>(point.x, point.y), particlesWorkingTexture);

		D3D12_TEXTURE_COPY_LOCATION src		= { 0 };
		src.Type				= D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.pResource				= texDataCPU.Get();		
		src.PlacedFootprint.Footprint.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
		src.PlacedFootprint.Footprint.Width	= WIDTH;
		src.PlacedFootprint.Footprint.Height	= HEIGHT;
		src.PlacedFootprint.Footprint.Depth	= 1;
		src.PlacedFootprint.Footprint.RowPitch	= WIDTH * 4;

		D3D12_TEXTURE_COPY_LOCATION dst		= { 0 };
		dst.Type				= D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.pResource				= texDataGPU.Get();
		dst.SubresourceIndex			= 0;		

		D3D12_BOX box				= { 0 };
		box.right				= WIDTH;
		box.bottom				= HEIGHT;
		box.back				= 1;

		commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, &box);
		ResourceBarrier(texDataGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

		ResourceBarrier(swapRTV[backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		ID3D12DescriptorHeap* descHeaps[] = { descHeapCBVSRV.Get(), descHeapSamplers.Get() };
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);
		commandList->ClearRenderTargetView(descHandle, color, 0, nullptr);
		commandList->OMSetRenderTargets(1, &descHandle, true, nullptr);		
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		commandList->SetDescriptorHeaps(2, descHeaps);
		commandList->SetGraphicsRootDescriptorTable(0, descHeapCBVSRV->GetGPUDescriptorHandleForHeapStart());
		commandList->SetGraphicsRootDescriptorTable(1, descHeapSamplers->GetGPUDescriptorHandleForHeapStart());
		commandList->SetPipelineState(pipelineState.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
 		ResourceBarrier(swapRTV[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->Close();

		ID3D12CommandList* const cl = commandList.Get();
		commandQueue->ExecuteCommandLists(1, &cl);

		fence->Signal(0);
		fence->SetEventOnCompletion(1, fenceEvent);
		commandQueue->Signal(fence.Get(), 1);
		WaitForSingleObject(fenceEvent, INFINITE);

		dxgiSwapChain->Present(1, 0);

		commandAllocator->Reset();
		commandList->Reset(commandAllocator.Get(), nullptr);
	}


	void CreateParticles()
	{
		// Generate a bunch of starting points for the particles		
		particles.Reserve(NUM_PARTICLES);

		foreach(i, particles.GetCapacity())
		{
			// Initial coordinate
			particles[i].X		 = static_cast<f32>(rand() % WIDTH);
			particles[i].Y		 = static_cast<f32>(rand() % HEIGHT);

			// So we know the distance traveled later
			particles[i].LastX	 = particles[i].X;
			particles[i].LastY	 = particles[i].Y;
		}

	}

	void AttractAndDrawParticles(Vector2<u32> point, void* textureData)
	{
		// Purge the texture first
		memset(particlesWorkingTexture, 0, (WIDTH* HEIGHT) * 4);

		// Attract / move the particles towards a point (in this case, the mouse)
		// Pythagorean distance 
		// distance = sqrtf(	pow(point.X - particles[i].X, 2) +
		//			pow(point.Y - particles[i].Y, 2));

		foreach(i, particles.GetCapacity())
		{
			f32 dx			= point.X - particles[i].X;
			f32 dy			= point.Y - particles[i].Y;
			f32 distance		= sqrtf((dx * dx) + (dy * dy));

			// Scale the amount to something sane and push... this is FPS dependent as is!
			particles[i].X		+= dx / distance;
			particles[i].Y		+= dy / distance;

			// The particles have moved because of the attraction right before
			// As the particle gets closer to the point, the difference naturally... 
			// ... shrinks and the velocity becomes smaller
			f32 velocityX		= particles[i].X - particles[i].LastX;
			f32 velocityY		= particles[i].Y - particles[i].LastY;
			particles[i].LastX	= particles[i].X;
			particles[i].LastY	= particles[i].Y;
			particles[i].X		+= velocityX;
			particles[i].Y		+= velocityY;

			// Skew the numbers for pretty colors
			ColorInt particleColor;
			f32 avgVel		= (velocityX + velocityY) / 2;
			f32 r			= 1.0f * abs(avgVel) / 2;
			f32 g			= 1.0f / abs(avgVel) * 2;
			f32 b			= 1.0f / abs(avgVel) / 4;

			particleColor.Red	= static_cast<u8>(Clamp(r * 255, 0, 255));
			particleColor.Blue	= static_cast<u8>(Clamp(b * 255, 0, 255));
			particleColor.Green	= static_cast<u8>(Clamp(g * 255, 0, 255));
			particleColor.Alpha	= 255;
		
			//SetPixelSafe(particles[i].X, particles[i].Y, particleColor, textureData);
			DrawLine(Round(particles[i].X), Round(particles[i].Y), Round(particles[i].LastX), Round(particles[i].LastY), particleColor, textureData);
		}
	}

private:
	// OODLES AND OODLES OF D3D STUFF
	ComPtr<ID3D12Device>			d3dDevice;
	ComPtr<IDXGIDevice1>			dxgiDevice;
	ComPtr<IDXGISwapChain3>			dxgiSwapChain;

	ComPtr<ID3D12CommandAllocator>		commandAllocator;
	ComPtr<ID3D12CommandQueue>		commandQueue;
	ComPtr<ID3D12GraphicsCommandList>	commandList;


	ComPtr<ID3D12PipelineState>		pipelineState;
	ComPtr<ID3D12RootSignature>		rootSignature;

	ComPtr<ID3D12DescriptorHeap>		descHeapRTV;
	ComPtr<ID3D12DescriptorHeap>		descHeapCBVSRV;
	ComPtr<ID3D12DescriptorHeap>		descHeapSamplers;
	ComPtr<ID3D12Resource>			swapRTV[2];

	ComPtr<ID3D12Resource>			vbibData;
	ComPtr<ID3D12Resource>			cbData;
	ComPtr<ID3D12Resource>			texDataCPU;
	ComPtr<ID3D12Resource>			texDataGPU;
	D3D12_VERTEX_BUFFER_VIEW		vbView;
	D3D12_INDEX_BUFFER_VIEW			ibView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC		cbView;
	D3D12_SHADER_RESOURCE_VIEW_DESC		texSRV;

	ComPtr<ID3D12Fence>			fence;
	HANDLE					fenceEvent;

	
	D3D12_VIEWPORT				viewport;
	D3D12_RECT				scissor;

	WindowHandle				handle;
	Array<Particle>				particles;
	void*					particlesWorkingTexture; // Always mapped
};


void TestApp::Run()
{
	window->Create("bnr", WIDTH, HEIGHT, WindowStyle::Normal);	// Typical window

	D3D12Test dx;
	dx.InitSwapChain(window->GetHandle());
	dx.InitResources();
	dx.InitData();

	dx.CreateParticles();	

	Timer timer;
	Event event;
	while (window->GetEvents(event))
	{
		UpdateWindowFrametimeInfo(*window, timer);
		dx.Draw();
	}
}



int main()
{
	TestApp app;
	app.Run();

	return 0;
}