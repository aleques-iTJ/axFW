#include "bnr.h"



#if defined (BNR_DEBUG)
	#if !defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Debug-x86.lib")
		#pragma comment(lib, "Renderer-Debug-x86.lib")
	#elif defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Debug-x64.lib")
		#pragma comment(lib, "Renderer-Debug-x64.lib")
	#endif
#else
	#if !defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Release-x86.lib")
		#pragma comment(lib, "Renderer-Release-x86.lib")
	#elif defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "Core-Release-x64.lib")
		#pragma comment(lib, "Renderer-Release-x64.lib")
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
	Renderer*	renderer;
};


TestApp::TestApp()
{
	window		= new Window;
	renderer	= new Renderer;
}

TestApp::~TestApp()
{
	// Reverse order from init
	// if (renderer)	delete renderer;
	if (window)		delete window;
}

/*#include "bnr.h"

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
	if (window)		delete window;
}


//==============================================================================================
// Huge fuckoff wall of shit incoming
//==============================================================================================

struct PCTVertex
{
	f32	x, y, z;	// 12 bytes, the literal position in pixels
	f32 r, g, b;	// 24 bytes, color to be modulated against
	f32	u, v;		// 32 bytes, tex coordinate
};


const u32 WIDTH			= 512;
const u32 HEIGHT		= 512;

const u32 MAX_INDICIES	= 65532; // Just about enough to fill a 16-bit index buffer
const u32 MAX_SPRITES	= MAX_INDICIES / 6;
const u32 MAX_VERTS		= MAX_SPRITES * 4;
const u32 MAX_SIZE		= MAX_VERTS * sizeof(PCTVertex);
const u16 VERTS_PER		= 4;
const u16 INDICIES_PER	= 6;

#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")

class D3D12Test
{
public:
	
	// Helper so this is less obnoxious to type
	void ResourceBarrier(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
	{
		D3D12_RESOURCE_BARRIER desc;
		memset(&desc, 0, sizeof(D3D12_RESOURCE_BARRIER));

		desc.Type						= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		desc.Transition.pResource		= res;
		desc.Transition.Subresource		= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		desc.Transition.StateBefore		= before;
		desc.Transition.StateAfter		= after;
		desc.Flags						= D3D12_RESOURCE_BARRIER_FLAG_NONE;
		commandList->ResourceBarrier(1, &desc);
	}


	// Take a guess
	bool Init(WindowHandle handle, u32 width, u32 height)
	{
		if (handle == nullptr)
			return false; // Invalid window handle


		// Drivers haven't updated yet, need to create a WARP device...
		IDXGIFactory4*	dxgiFactory	= nullptr;
		IDXGIAdapter3*	adapater;
		CreateDXGIFactory2(0, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&dxgiFactory));
		dxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter3), reinterpret_cast<void**>(&adapater));
		D3D12CreateDevice(adapater, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), reinterpret_cast<void**>(&d3dDevice));
		
		
		d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&commandAllocator));
		D3D12_COMMAND_QUEUE_DESC cqDesc;
		cqDesc.Type				= D3D12_COMMAND_LIST_TYPE_DIRECT;
		cqDesc.Priority			= 0;
		cqDesc.Flags			= D3D12_COMMAND_QUEUE_FLAG_NONE;
		cqDesc.NodeMask			= 0;
		d3dDevice->CreateCommandQueue(&cqDesc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&commandQueue));
		d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), reinterpret_cast<void**>(&commandList));


		// Ffor the swap chain render targets
		D3D12_DESCRIPTOR_HEAP_DESC dhDesc;
		dhDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		dhDesc.NumDescriptors	= 8;
		dhDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dhDesc.NodeMask			= 0;
		d3dDevice->CreateDescriptorHeap(&dhDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&descHeapRTV));

		// For the projection matrix cbuffer and the texture SRV
		dhDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		dhDesc.NumDescriptors	= 8;
		dhDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		dhDesc.NodeMask			= 0;
		d3dDevice->CreateDescriptorHeap(&dhDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&descHeapCBVSRV));

		// For the texture sampler
		dhDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		dhDesc.NumDescriptors	= 8;
		dhDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		dhDesc.NodeMask			= 0;
		d3dDevice->CreateDescriptorHeap(&dhDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&descHeapSamplers));


		D3D12_DESCRIPTOR_RANGE descRangeCBVSRV[2];
		memset(descRangeCBVSRV, 0, sizeof(descRangeCBVSRV));
		descRangeCBVSRV[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		descRangeCBVSRV[0].NumDescriptors						= 1;
		descRangeCBVSRV[0].BaseShaderRegister					= 0;
		descRangeCBVSRV[0].RegisterSpace						= 0;
		descRangeCBVSRV[0].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		descRangeCBVSRV[1].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descRangeCBVSRV[1].NumDescriptors						= 1;
		descRangeCBVSRV[1].BaseShaderRegister					= 0;
		descRangeCBVSRV[1].RegisterSpace						= 0;
		descRangeCBVSRV[1].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_DESCRIPTOR_RANGE descRangeSamplers;
		memset(&descRangeSamplers, 0, sizeof(descRangeSamplers));
		descRangeSamplers.RangeType								= D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		descRangeSamplers.NumDescriptors						= 1;
		descRangeSamplers.BaseShaderRegister					= 0;
		descRangeSamplers.RegisterSpace							= 0;
		descRangeSamplers.OffsetInDescriptorsFromTableStart		= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		D3D12_ROOT_PARAMETER rootParam[2];
		rootParam[0].ParameterType								= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParam[0].DescriptorTable.NumDescriptorRanges		= 2;
		rootParam[0].DescriptorTable.pDescriptorRanges			= descRangeCBVSRV;
		rootParam[0].ShaderVisibility							= D3D12_SHADER_VISIBILITY_ALL;
		rootParam[1].ParameterType								= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParam[1].DescriptorTable.NumDescriptorRanges		= 1;
		rootParam[1].DescriptorTable.pDescriptorRanges			= &descRangeSamplers;
		rootParam[1].ShaderVisibility							= D3D12_SHADER_VISIBILITY_PIXEL;

		ID3D10Blob*					signature					= nullptr;
		D3D12_ROOT_SIGNATURE_DESC	signatureDesc				= { 0 };		
		signatureDesc.NumParameters								= 2;
		signatureDesc.pParameters								= rootParam;		
		signatureDesc.Flags										= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
		d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(&rootSignature));
		signature->Release();


		// Start setting up DXGI and the swap chain... looks just like D3D11
		DXGI_SWAP_CHAIN_DESC scd;
		scd.BufferCount							= 2;
		scd.BufferDesc.Width					= WIDTH;
		scd.BufferDesc.Height					= HEIGHT;
		scd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator	= 60;	// Hmm...
		scd.BufferDesc.RefreshRate.Denominator	= 1;
		scd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
		scd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow						= handle;
		scd.SampleDesc.Count					= 1;
		scd.SampleDesc.Quality					= 0;
		scd.Windowed							= true;										// Always start in a windowed state
		scd.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		scd.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;


		// Apparently it takes the queue instead of the device
		// http://www.gamedev.net/topic/666986-direct3d-12-documentation-is-now-public/#entry5219717
		dxgiFactory->CreateSwapChain(commandQueue, &scd, reinterpret_cast<IDXGISwapChain**>(&dxgiSwapChain));
		dxgiFactory->Release();

		// Backbuffer		
		foreach(i, 2)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE descHandle = descHeapRTV->GetCPUDescriptorHandleForHeapStart();			
			descHandle.ptr += i * d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			dxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&swapRTV[i]));
			d3dDevice->CreateRenderTargetView(swapRTV[i], nullptr, descHandle);
		}

		d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(&fence));
		fenceEvent = CreateEvent(nullptr, false, false, nullptr);

		// Done with basic D3D12 init
//----------------------------------------------------------------------------------------------
		// At this point, we start doing the sprite rendering setup

		// Setup the input layout for a sprite
		D3D12_INPUT_ELEMENT_DESC elementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0								},
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0	},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0		}
		};

		D3D12_INPUT_LAYOUT_DESC inputLayout = { elementDesc, 3 };


		// Load and compile the sprite shader
		String shaderPath = System::GetExeDirectory();
		shaderPath.Append("BasicSprite_PCT.txt");

		File* shader = FileSystem::OpenFile(shaderPath, FileAccess::Read);
		FileSystem::ReadFile(shader);

		ID3D10Blob*	vsBuffer = nullptr;
		ID3D10Blob*	psBuffer = nullptr;
		D3DCompile(shader->GetDataPtr(), shader->GetSize(), nullptr, nullptr, nullptr, "vsMain", "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsBuffer, nullptr);
		D3DCompile(shader->GetDataPtr(), shader->GetSize(), nullptr, nullptr, nullptr, "psMain", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psBuffer, nullptr);

		// Whyyy MS did you remove the CD3D12 helpers what is this
		D3D12_RASTERIZER_DESC rasterDesc;
		memset(&rasterDesc, 0, sizeof(D3D12_RASTERIZER_DESC));
		rasterDesc.FillMode					 = D3D12_FILL_MODE_SOLID;
		rasterDesc.CullMode					 = D3D12_CULL_MODE_NONE;


		D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
		memset(&depthStencilDesc, 0, sizeof(D3D12_DEPTH_STENCIL_DESC));
		const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		depthStencilDesc.DepthEnable		= TRUE;
		depthStencilDesc.DepthWriteMask		= D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc			= D3D12_COMPARISON_FUNC_LESS;
		depthStencilDesc.StencilReadMask	= D3D12_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask	= D3D12_DEFAULT_STENCIL_WRITE_MASK;
		depthStencilDesc.FrontFace			= defaultStencilOp;
		depthStencilDesc.BackFace			= defaultStencilOp;


		// ty msdn
		D3D12_BLEND_DESC blendDesc;
		memset(&blendDesc, 0, sizeof(D3D12_BLEND_DESC));
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL
		};

		blendDesc.RenderTarget[0] = defaultRenderTargetBlendDesc;

		// OK GOD
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		ZeroMemory(&psoDesc, sizeof(psoDesc));
		psoDesc.VS.pShaderBytecode				= vsBuffer->GetBufferPointer();
		psoDesc.VS.BytecodeLength				= vsBuffer->GetBufferSize();
		psoDesc.PS.pShaderBytecode				= psBuffer->GetBufferPointer();
		psoDesc.PS.BytecodeLength				= psBuffer->GetBufferSize();
		psoDesc.SampleDesc.Count				= 1;
		psoDesc.SampleMask						= UINT_MAX;
		psoDesc.InputLayout.pInputElementDescs	= elementDesc;
		psoDesc.InputLayout.NumElements			= 3;
		psoDesc.pRootSignature					= rootSignature;
		psoDesc.NumRenderTargets				= 1;
		psoDesc.RTVFormats[0]					= DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.PrimitiveTopologyType			= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RasterizerState					= rasterDesc;
		psoDesc.DepthStencilState				= depthStencilDesc;
		psoDesc.BlendState						= blendDesc;
			
		d3dDevice->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&pipelineState));
		vsBuffer->Release();
		psBuffer->Release();

//----------------------------------------------------------------------------------------------

		D3D12_HEAP_PROPERTIES heapProperties;
		memset(&heapProperties, 0, sizeof(D3D12_HEAP_PROPERTIES));
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;


		// Sweet jesus
		PCTVertex verts[4];
		verts[0].x = 0.0f;		// Bottom left
		verts[0].y = HEIGHT;
		verts[0].z = 1.0f;
		verts[0].r = 1.0f;
		verts[0].g = 1.0f;
		verts[0].b = 1.0f;
		verts[0].u = 0.0f;
		verts[0].v = 1.0f;
		verts[1].x = 0.0f;		// Top left
		verts[1].y = 0.0f;
		verts[1].z = 1.0f;
		verts[1].r = 1.0f;
		verts[1].g = 1.0f;
		verts[1].b = 1.0f;
		verts[1].u = 0.0f;
		verts[1].v = 0.0f;
		verts[2].x = WIDTH;		// Bottom right
		verts[2].y = HEIGHT;
		verts[2].z = 1.0f;
		verts[2].r = 1.0f;
		verts[2].g = 1.0f;
		verts[2].b = 1.0f;
		verts[2].u = WIDTH;
		verts[2].v = HEIGHT;
		verts[3].x = WIDTH;		// Top right
		verts[3].y = 0.0f;
		verts[3].z = 1.0f;
		verts[3].r = 1.0f;
		verts[3].g = 1.0f;
		verts[3].b = 1.0f;
		verts[3].u = WIDTH;
		verts[3].v = 0.0f;


		// Indicies for the... index buffer
		u16 indicies[6];
		indicies[0] = 0;	// Bottom left
		indicies[1] = 1;	// Top left
		indicies[2] = 2;	// Bottom right
		indicies[3] = 2;	// Bottom right
		indicies[4] = 1;	// Top left
		indicies[5] = 3;	// Top right

		// VB and IB
		D3D12_RESOURCE_DESC vbibDesc;
		memset(&vbibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		vbibDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
		vbibDesc.Width				= sizeof(verts) + sizeof(indicies);
		vbibDesc.Height				= 1;
		vbibDesc.DepthOrArraySize	= 1;
		vbibDesc.MipLevels			= 1;
		vbibDesc.SampleDesc.Count	= 1;
		vbibDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3dDevice->CreateCommittedResource(	&heapProperties, D3D12_HEAP_FLAG_NONE, &vbibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, 
											nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&geometryData));

		// Move in the vertex and index data
		char* ptr = nullptr;
		geometryData->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
		memcpy(ptr, verts, sizeof(verts));							// VB first
		memcpy(ptr + sizeof(verts), indicies, sizeof(indicies));	// IB next
		geometryData->Unmap(0, nullptr);

		vbView.BufferLocation		= geometryData->GetGPUVirtualAddress();
		vbView.StrideInBytes		= sizeof(PCTVertex);
		vbView.SizeInBytes			= sizeof(verts);
		ibView.BufferLocation		= geometryData->GetGPUVirtualAddress() + sizeof(verts);
		ibView.Format				= DXGI_FORMAT_R16_UINT;
		ibView.SizeInBytes			= sizeof(indicies);


		// Cbuffer
		D3D12_RESOURCE_DESC cbDesc;
		memset(&cbDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		cbDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
		cbDesc.Width				= D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;	// Apparently it must be a multiple of 256
		cbDesc.Height				= 1;
		cbDesc.DepthOrArraySize		= 1;
		cbDesc.MipLevels			= 1;
		cbDesc.SampleDesc.Count		= 1;
		cbDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		d3dDevice->CreateCommittedResource(	&heapProperties, D3D12_HEAP_FLAG_NONE, &cbDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
											nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&cbuffer));

		Matrix proj	= Matrix::Ortho2D(static_cast<FLOAT>(WIDTH), static_cast<FLOAT>(HEIGHT), 0.1f, 100.0f);
		cbuffer->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
		memcpy(ptr, &proj, sizeof(Matrix));
		cbuffer->Unmap(0, nullptr);

		cbView.BufferLocation	= cbuffer->GetGPUVirtualAddress();
		cbView.SizeInBytes		= D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		d3dDevice->CreateConstantBufferView(&cbView, descHeapCBVSRV->GetCPUDescriptorHandleForHeapStart());

		
		// Texture
		D3D12_RESOURCE_DESC texDesc;
		memset(&texDesc, 0, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Format							= DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.Dimension						= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Width							= WIDTH;	// Apparently it must be a multiple of 256
		texDesc.Height							= HEIGHT;
		texDesc.DepthOrArraySize				= 1;
		texDesc.MipLevels						= 1;
		texDesc.SampleDesc.Count				= 1;
		texDesc.Layout							= D3D12_TEXTURE_LAYOUT_UNKNOWN;
		d3dDevice->CreateCommittedResource(	&heapProperties, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
											nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&texture));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format							= DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels				= 1; // No MIP
		srvDesc.Texture2D.MostDetailedMip		= 0;
		srvDesc.Texture2D.PlaneSlice			= 0;
		srvDesc.Texture2D.ResourceMinLODClamp	= 0.0f;
		d3dDevice->CreateShaderResourceView(texture, &srvDesc, descHeapCBVSRV->GetCPUDescriptorHandleForHeapStart());

		D3D12_SAMPLER_DESC samplerDesc;
		memset(&samplerDesc, 0, sizeof(D3D12_SAMPLER_DESC));
		samplerDesc.Filter			= D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.MinLOD			= -FLT_MAX;
		samplerDesc.MaxLOD			= FLT_MAX;
		samplerDesc.ComparisonFunc	= D3D12_COMPARISON_FUNC_NEVER;
		d3dDevice->CreateSampler(&samplerDesc, descHeapSamplers->GetCPUDescriptorHandleForHeapStart());


//----------------------------------------------------------------------------------------------

		
		// Set the viewport and fill the cbuffer with the projection matrix
		viewport.MaxDepth		= 1.0f;
		viewport.MinDepth		= 0.0f;
		viewport.TopLeftX		= 0.0f;
		viewport.TopLeftY		= 0.0f;
		viewport.Width			= static_cast<FLOAT>(WIDTH);
		viewport.Height			= static_cast<FLOAT>(HEIGHT);

		// Why did they randomly change case here?
		scissor.top				= 0;
		scissor.left			= 0;
		scissor.right			= WIDTH;
		scissor.bottom			= HEIGHT;

		return true;
	}

	void Draw()
	{		
		D3D12_CPU_DESCRIPTOR_HANDLE descHandle		= descHeapRTV->GetCPUDescriptorHandleForHeapStart();
		UINT						backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
									descHandle.ptr += backBufferIndex * d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		f32							color[]			= { 0.0f, 0.3f, 0.0f, 0.0f };		

		ResourceBarrier(swapRTV[backBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);
		commandList->ClearRenderTargetView(descHandle, color, 0, nullptr);
		commandList->OMSetRenderTargets(1, &descHandle, true, nullptr);

		ID3D12DescriptorHeap* descHeaps[] = { descHeapCBVSRV, descHeapSamplers };
		commandList->SetGraphicsRootSignature(rootSignature);
		commandList->SetDescriptorHeaps(2, descHeaps);
		commandList->SetGraphicsRootDescriptorTable(0, descHeapCBVSRV->GetGPUDescriptorHandleForHeapStart());
		commandList->SetGraphicsRootDescriptorTable(1, descHeapSamplers->GetGPUDescriptorHandleForHeapStart());
		commandList->SetPipelineState(pipelineState);
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);
		commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);		

 		ResourceBarrier(swapRTV[backBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->Close();

		ID3D12CommandList* const cl = commandList;
		commandQueue->ExecuteCommandLists(1, &cl);

		fence->Signal(0);
		fence->SetEventOnCompletion(1, fenceEvent);
		commandQueue->Signal(fence, 1);
		WaitForSingleObject(fenceEvent, INFINITE);

		dxgiSwapChain->Present(1, 0);

		commandAllocator->Reset();
		commandList->Reset(commandAllocator, nullptr);
	}

	

private:
	// OODLES AND OODLES OF D3D STUFF
	ComPtr<ID3D12Device>				d3dDevice;
	ComPtr<IDXGIDevice2>				dxgiDevice;
	ComPtr<IDXGISwapChain3>				dxgiSwapChain;

	ComPtr<ID3D12CommandAllocator>		commandAllocator;
	ComPtr<ID3D12CommandQueue>			commandQueue;
	ComPtr<ID3D12GraphicsCommandList>	commandList;

	ComPtr<ID3D12PipelineState>			pipelineState;
	ComPtr<ID3D12RootSignature>			rootSignature;

	ComPtr<ID3D12DescriptorHeap>		descHeapRTV;
	ComPtr<ID3D12DescriptorHeap>		descHeapCBVSRV;
	ComPtr<ID3D12DescriptorHeap>		descHeapSamplers;
	ComPtr<ID3D12Resource>				swapRTV[2];

	ComPtr<ID3D12Fence>					fence;
	HANDLE								fenceEvent;

	ComPtr<ID3D12Resource>				geometryData;
	ComPtr<ID3D12Resource>				cbuffer;
	ComPtr<ID3D12Resource>				texture;
	D3D12_VERTEX_BUFFER_VIEW			vbView;
	D3D12_INDEX_BUFFER_VIEW				ibView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC		cbView;
	D3D12_SHADER_RESOURCE_VIEW_DESC		texSRV;

	D3D12_VIEWPORT						viewport;
	D3D12_RECT							scissor;
};



void UpdateWindowFrametimeInfo(Window& window, Timer& timer)
{
	timer.Update();
	f64 hz = timer.GetHz();
	f64 ms = timer.GetMs();
	f64 ac = timer.GetAccumulator();

	if (ac >= 1000.0f)
	{
		char str[128];
		sprintf(str, "- FPS:%f; MS:%f", hz, ms);
		SetWindowText(window.GetWindowHandle(), str);

		timer.ResetAccumuator();
	}
}


void TestApp::Run()
{
	window->Create("bnr", WIDTH, HEIGHT, Window::Style::Normal);	// Typical window

	D3D12Test dx;

	dx.Init(window->GetWindowHandle(), WIDTH, HEIGHT);

	Timer timer;
	Timer gameTimer;
	Event event;
	while (window->GetEvents(event))
	{
		gameTimer.Update();
		UpdateWindowFrametimeInfo(*window, timer);

		dx.Draw();
	}
}



int main()
{
	TestApp app;
	app.Run();

	return 0;
}*/