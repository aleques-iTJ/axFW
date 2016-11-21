#pragma once
#include "Renderer\Backend\ShaderBase.h"
#include "RendererDX11\Backend\InputTypesDX11.h"
#include "RendererDX11\EnumsDX11.h"
#include <d3d11.h>

enum class DataType
{
	PerVertex = D3D11_INPUT_PER_VERTEX_DATA,
	PerInstance = D3D11_INPUT_PER_INSTANCE_DATA
};

//----------------------------------------------------------------

struct BNR_API ShaderDX11 final : public ShaderBase
{
	friend class RendererDX11;

	//----------------------------------------------------------------

public:
	ShaderDX11::ShaderDX11() :
		vs		(nullptr),
		ps		(nullptr),
		vsBuffer(nullptr),
		psBuffer(nullptr),
		layout	(nullptr)
	{

	}

	ShaderDX11::~ShaderDX11()
	{

	}

//----------------------------------------------------------------

	ShaderType			ShaderType;
	InputLayout::Type	InputType;

//----------------------------------------------------------------

	void LoadFromFile(const String& path, bool compile = true);
	void LoadFromMemory(void* ptr);
	void LoadFromMemory(const String& shader);

	void Compile();
	void Activate();

	//----------------------------------------------------------------

private:
	RendererDX11* renderer;

	ComPtr<ID3D11VertexShader>	vs;
	ComPtr<ID3D11PixelShader>	ps;
	ComPtr<ID3D10Blob>			vsBuffer;
	ComPtr<ID3D10Blob>			psBuffer;

	ID3D11InputLayout*			layout;
};