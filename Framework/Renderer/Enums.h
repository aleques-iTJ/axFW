#pragma once
#include "bnrConfig.h"

//----------------------------------------------------------------

#ifdef BNR_RENDERER_DX11



#elif BNR_RENDERER_GL

#include "RendererGL\EnumsGL.h"
#include "RendererGL\TypesGL.h"

#endif


#pragma once

// Macros for abstracted renderer stuff
// Less ifdef stuff and crap to pollute the object headers
// and less duplicate stuff floating around the API specific objects
#if defined (BNR_RENDERER_DX11)
	#define Renderer_H "Video\Renderer\DX11\RendererDX11.h"
	#define R_Object(object_name) object_name##DX11
#endif

#if defined (BNR_RENDERER_GL)
	#define Renderer_H "Video\Renderer\GL\RendererGL.h"
	#define R_Object(object_name) object_name##GL
#endif

// Some of these names aren't the best, some can be a bit smarter

#ifdef BNR_RENDERER_DX11
#include <d3d11.h>

enum class DataFormat
{
	// Typical texture formats
	Unknown			= 0,							// This is probably an error!
	RGBA			= DXGI_FORMAT_R8G8B8A8_UNORM,
	BGRA			= DXGI_FORMAT_B8G8R8A8_UNORM,
	DXT1			= DXGI_FORMAT_BC1_UNORM,
	DXT3			= DXGI_FORMAT_BC3_UNORM,
	DXT5			= DXGI_FORMAT_BC5_UNORM,
	Depth			= DXGI_FORMAT_D32_FLOAT,
	DepthStencil	= DXGI_FORMAT_D24_UNORM_S8_UINT,
	RenderTarget	= 1 << 30,						// What was I even going for here?

	// Index buffer type
	Indicies16Bit	= DXGI_FORMAT_R16_UINT,
	Indicies32Bit	= DXGI_FORMAT_R32_UINT,

	// Input layout component type
	Vector1			= DXGI_FORMAT_R32_FLOAT,
	Vector2			= DXGI_FORMAT_R32G32_FLOAT,
	Vector3			= DXGI_FORMAT_R32G32B32_FLOAT,
	Vector4			= DXGI_FORMAT_R32G32B32A32_FLOAT
};

enum class TextureDimension
{
	OneDimensional		= 1,	// This is tricky with DX...
	TwoDimensional		= 2,	// the flags are separate for Depth stencil and render targets!
	ThreeDimensional	= 3		// RTV need to have 1 added to them!
};

enum class FilterMode
{
	Nearest			= D3D11_FILTER_MIN_MAG_MIP_POINT,
	Trilinear		= D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	Anisotropic		= D3D11_FILTER_ANISOTROPIC
};

enum class AddressMode
{
	Repeat	= D3D11_TEXTURE_ADDRESS_WRAP,
	Mirror	= D3D11_TEXTURE_ADDRESS_MIRROR,
	Clamp	= D3D11_TEXTURE_ADDRESS_CLAMP,
};

enum class UsagePattern	// The expected usage pattern
{
	Default		= D3D11_USAGE_DEFAULT,
	Immutable	= D3D11_USAGE_IMMUTABLE,
	Static		= D3D11_USAGE_DEFAULT,
	Dynamic		= D3D11_USAGE_DYNAMIC,
	Staging		= D3D11_USAGE_STAGING
};

enum class BindFlags	// What it's bound as
{
	Unknown				= 0,
	VertexBuffer		= D3D11_BIND_VERTEX_BUFFER,
	IndexBuffer			= D3D11_BIND_INDEX_BUFFER,
	ConstantBuffer		= D3D11_BIND_CONSTANT_BUFFER,
	ShaderResource		= D3D11_BIND_SHADER_RESOURCE,
	StreamOutput		= D3D11_BIND_STREAM_OUTPUT,
	RenderTarget		= D3D11_BIND_RENDER_TARGET,
	DepthStencil		= D3D11_BIND_DEPTH_STENCIL,
	UnorderedAccess		= D3D11_BIND_UNORDERED_ACCESS
};

enum class ReadWriteAccess
{
	Default		= 0,
	Write		= D3D11_CPU_ACCESS_WRITE,
	Read		= D3D11_CPU_ACCESS_READ,
	ReadWrite	= D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ
};

enum class DepthWriteMask
{
	Disable	= D3D11_DEPTH_WRITE_MASK_ZERO,
	Enable	= D3D11_DEPTH_WRITE_MASK_ALL
};

enum class CompareFunc
{
	Never			= D3D11_COMPARISON_NEVER,
	Less			= D3D11_COMPARISON_LESS,
	Equal			= D3D11_COMPARISON_EQUAL,
	LessEqual		= D3D11_COMPARISON_LESS_EQUAL,
	Greate			= D3D11_COMPARISON_GREATER,
	NotEqual		= D3D11_COMPARISON_NOT_EQUAL,
	GreaterEqual	= D3D11_COMPARISON_GREATER_EQUAL,
	Always			= D3D11_COMPARISON_ALWAYS
};

enum class StencilOp
{
	Keep		= D3D11_STENCIL_OP_KEEP,
	Zero		= D3D11_STENCIL_OP_ZERO,
	Replace		= D3D11_STENCIL_OP_REPLACE,
	IncreaseSat = D3D11_STENCIL_OP_INCR_SAT,
	DecreaseSat = D3D11_STENCIL_OP_DECR_SAT,
	Invert		= D3D11_STENCIL_OP_INVERT,
	Increase	= D3D11_STENCIL_OP_INCR,
	Decrease	= D3D11_STENCIL_OP_DECR
};

enum class FillMode
{
	Wireframe	= D3D11_FILL_WIREFRAME,
	Solid		= D3D11_FILL_SOLID
};

enum class CullMode
{
	None		= D3D11_CULL_NONE,
	Front		= D3D11_CULL_FRONT,
	Back		= D3D11_CULL_BACK
};

enum class PrimitiveType
{
	Point			= D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	LineList		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	LineStrip		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	TriangleList	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TriangleStrip	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

enum class PipelineStage
{
	Unknown			= 0, // Probably an error...
	VertexShader	= 1 << 1,
	PixelShader		= 1 << 2,
	GeometryShader	= 1 << 3,
};

#endif

#ifdef BNR_RENDERER_GL
#pragma once
#include "Extern\gl\glcorearb.h"
#include "Extern\gl\wglext.h"

#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_STREAM_DRAW 0x88E0
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

enum class Format
{
	Unknown			= 0,		// This is probably an error!
	RGBA			= 1 << 1,
	DXT1			= 1 << 2,
	DXT3			= 1 << 3,
	DXT5			= 1 << 4,
	Depth			= 1 << 5,
	Stencil			= 0,		// Does an explicit stencil format exist? Is there a point?
	DepthStencil	= 1 << 7,
	RenderTarget	= 1 << 30,	// What was I even going for here?
	BNRTex			= 1 << 31	// LZ4 compressed -- RGBA
};

enum class TextureDimension
{
	OneDimensional		= 1,
	TwoDimensional		= 2,
	ThreeDimensional	= 3
};

enum class FilterMode
{
	Nearest		= GL_NEAREST,
	Trilinear	= GL_LINEAR,
	Anisotropic	= GL_TEXTURE_MAX_ANISOTROPY_EXT
};

enum class AddressMode
{
	Repeat	= GL_REPEAT,
	Mirror	= 1, //,
	Clamp	= GL_CLAMP_TO_EDGE,
};

enum class Usage // The expected usage pattern
{
	Immutable	= GL_STATIC_DRAW,
	Static		= GL_STATIC_DRAW,
	Dynamic		= GL_DYNAMIC_DRAW,
	Staging		= 0x8000
};

enum class BindFlags	// What it's bound as
{
	Unknown				= 0,
	VertexBuffer		= 1 << 1,
	IndexBuffer			= 1 << 2,
	ConstantBuffer		= 1 << 3,
	ConstantBufferVS	= 1 << (24 + 1),			// Special case, used as a flag to internally signify that it
	ConstantBufferPS	= 1 << (24 + 2),			// should be bound to that specific part of the pipeline
	ShaderResource		= 1 << 4,
	ShaderResourceVS	= 1 << (24 + 3),			// ^ ditto
	ShaderResourcePS	= 1 << (24 + 4),
	StreamOutput		= 1 << 5,
	RenderTarget		= 1 << 6,
	DepthStencil		= 1 << 7,
	UnorderedAccess		= 1 << 8,
};

enum class ReadWriteAccess
{
	Default		= 0,
	WriteAccess = 0,
	ReadAccess	= 0
};

enum class DepthWriteMask
{
	Disable	= GL_FALSE,
	Enable	= GL_TRUE
};

enum class CompareFunc
{
	Never			= GL_NEVER,
	Less			= GL_LESS,
	LessEqual		= GL_LEQUAL,
	Equal			= GL_EQUAL,
	NotEqual		= GL_NOTEQUAL,
	Greater			= GL_GREATER,
	GreaterEqual	= GL_GEQUAL,
	Always			= GL_ALWAYS
};

enum class StencilOp
{
	Replace		= GL_REPLACE,
	Keep		= GL_KEEP,
	Zero		= GL_ZERO,
	IncreaseSat = GL_INCR,
	DecreaseSat = GL_DECR,
	Invert		= GL_INVERT,
	Increase	= GL_INCR_WRAP,
	Decrease	= GL_DECR_WRAP
};

enum class FillMode
{
	Wireframe	= GL_LINE,
	Solid		= GL_FILL
};

enum class CullMode
{
	None	= 0,
	Front	= GL_FRONT,
	Back	= GL_BACK
};
#endif