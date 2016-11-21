#pragma once
#include "Renderer\Base\RendererBase.h"
#include "Renderer\Enums.h"
#include <d3d11.h>

#include "Core/Containers/Array.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//----------------------------------------------------------------

struct TextureDX11;
struct BufferDX11;
struct ShaderDX11;

class BNR_API RendererDX11 : public RendererBase
{
	RendererDX11 ();
	~RendererDX11();

	// Setup
	bool Create		(WindowHandle handle, u32 width, u32 height);
	bool Destroy	();

	// ... Renderer stuff
	void PreRender	();			// Rendering setup, like texture binding, etc
	void PostRender	() const;
	void Clear		() const;
	void Present	() const;	// Just flip buffers

	ResourceID CreateBuffer	(BufferDX11& buffer);
	ResourceID CreateTexture(TextureDX11& texture);
	ResourceID CreateShader	(ShaderDX11& shader);

private:
	// Some raw D3D stuff
	ComPtr<IDXGISwapChain>		swapChain;
	ComPtr<ID3D11Device>		device;
	ComPtr<ID3D11DeviceContext>	context;

	TextureDX11*				backBuffer;
	TextureDX11*				depthBuffer;

	// Created resources, a ResourceID is an index to the given array
	Array<TextureDX11*>			managedTextures;	
	Array<BufferDX11*>			managedBuffers;
	Array<ShaderDX11*>			managedShaders;
};