#pragma once
#include "Core/Containers/Array.h"
#include "Core/Containers/String.h"


using BufferID	= uint16;
using TextureID = uint16;
using ShaderID	= uint16;

enum class CommandListType
{
	Default	 = 0,
	Graphics = 0,
	Compute,
	Copy
};

/*struct PipelineState
{
	ShaderID		vertexShader;
	ShaderID		pixelShader;

	D3D12_BLEND_DESC              BlendState;
	D3D12_RASTERIZER_DESC         RasterizerState;
	D3D12_DEPTH_STENCIL_DESC      DepthStencilState;
	InputLayout					InputLayout;
	TopologyType				PrimitiveTopologyType;
	uint8						NumRenderTargets;
	DXGI_FORMAT					RTVFormats[8];
	DXGI_SAMPLE_DESC            SampleDesc;
}*/

enum class MemoryType
{
	DeviceLocal,	// Likely GPU memory
	HostVisible		// Likely system memory
};

struct Memory
{
	const uint8_t*	Data	= nullptr;	// Initial data, null if not applicable 
	size_t			Size	= 0;
	size_t			Stride	= 0;
	size_t			Offset	= 0;

	MemoryType		Type	= MemoryType::DeviceLocal;
};

enum class BufferType
{
	Unknown,		// Error
	VertexBuffer,
	IndexBuffer,	// 16 bit by default
	ConstantBuffer,	// AKA Uniform buffer
	TransferBuffer	// AKA Staging buffer
};

struct BufferInfo
{
	BufferType	Type					= BufferType::Unknown;
	bool		Use32BitIndicies		= false;
	bool		UseAsIndirectDrawBuffer = false;
};

enum class TextureType
{
	OneDimensional,
	TwoDimensional,
	ThreeDimensional,

	RenderTarget,	// These are both implicitly 2D.
	DepthStencil
};

enum class TextureFormat
{
	// Typical texture formats
	Unknown,
	RGBA,
	DXT1,
	DXT5
};

struct TextureInfo
{
	TextureType		Type	= TextureType::TwoDimensional;
	TextureFormat	Format	= TextureFormat::Unknown;

	uint32	Width	= 1;
	uint32	Height	= 1;
	uint32	Depth	= 1;
	uint8	NumMips = 1;
};





class CommandList;
struct VertexDecl;

class Device
{
	friend class Renderer;

public:
	bool		Create			(uint32 width, uint32 height, void* handle);

	BufferID	CreateBuffer	(Memory* const memory, BufferInfo info);
	TextureID	CreateTexture	(Memory* const memory, TextureInfo info);
	//ShaderID	CreateShader	(ShaderDX11& shader);
	
	//void		CreateCommandList(const CommandList* const commandList, CommandListType type = CommandListType::Default);
	void Frame(uint8 test);




	String Name			= "Invalid adapter";
	size_t TotalMemory	= 0;

private:	
	struct NativeCache;
	NativeCache* cache;

	int8 pfData[64];
};