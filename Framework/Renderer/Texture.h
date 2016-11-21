#pragma once
#include "Renderer\Resource.h"


enum class TextureDimension
{
	OneDimensional,
	TwoDimensional,
	ThreeDimensional,
};

enum class FilterMode
{
	Nearest,
	Trilinear,
	Anisotropic,
};

enum class AddressMode
{
	Repeat,
	Mirror,
	Clamp,
};

struct Texture : public Resource
{
public:
	void FromFile(const File& file);

//----------------------------------------------------------------

	TextureDimension	Dimension;
	FilterMode			Filter;
	AddressMode			WrapMode;

	uint32				Width;
	uint32				Height;
	uint32				Depth;

	uint32				NumFrames;
	uint32				FrameRate;
	uint32				MipMapCount;
};