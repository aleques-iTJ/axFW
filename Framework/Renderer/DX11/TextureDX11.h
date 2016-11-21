#pragma once
#include "Renderer/Enums.h"

#include "Renderer/Base/TextureBase.h"

struct BNR_API TextureDX11 : public TextureBase
{
	friend class RendererDX11;

//----------------------------------------------------------------

public:
	TextureDX11::TextureDX11(bool defaults = false)
	{
		if (defaults == true)
			SetDefaults();
	}

	TextureDX11::~TextureDX11()
	{

	}

//----------------------------------------------------------------

	void SetDefaults()
	{
		// Base Resource
		//Subresource	= nullptr;
		Format			= DataFormat::RGBA;
		CPUAccessType	= ReadWriteAccess::Default;
		BindAs			= BindFlags::ShaderResource;
		Usage			= UsagePattern::Static;

		// Base Texture
		Dimension		= TextureDimension::TwoDimensional;
		Filter			= FilterMode::Trilinear;
		WrapMode		= AddressMode::Repeat;
		Width			= 1;
		Height			= 1;
		Depth			= 0;

		NumFrames		= 1;
		FrameRate		= 0;
		MipMapCount		= 1;
	}

//----------------------------------------------------------------

private:
	ComPtr<ID3D11Texture2D>				tex;

	ComPtr<ID3D11ShaderResourceView>	srv;
	ComPtr<ID3D11RenderTargetView>		rtv;
	ComPtr<ID3D11DepthStencilView>		dsv;
};