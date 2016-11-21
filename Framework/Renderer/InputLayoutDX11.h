#pragma once
#include "Types.h"
#include <d3d11.h>


namespace InputLayout
{
	enum class Type
	{
		PositionColor,
		PositionColorTexcoord
	};


//==============================================================================================
// Predefined input layout types
//==============================================================================================

	// Position, color, texcoord - useful for 2D
	struct PCTVertex
	{
		float32 x, y, z;	// 12 bytes, the literal position in pixels
		float32 r, g, b;	// 24 bytes, color to be modulated against
		float32 u, v;		// 32 bytes, tex coordinate


		static const D3D11_INPUT_ELEMENT_DESC* GetInputDesc()
		{
			D3D11_INPUT_ELEMENT_DESC inputLayout[] =
			{
				{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			return inputLayout;
		}
	};
}