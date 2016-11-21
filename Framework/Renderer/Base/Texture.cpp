#pragma once
#include "Renderer\Texture.h"
#include "Core\FileSystem\File.h"

bool Texture::FromFile(const File& file)
{
	const u8* headerData = static_cast<const uint8*>(file.GetDataPtr());

	// Check if it's actually a bnrFW texture file
	uint32 id	= *reinterpret_cast<const uint32*>(&headerData[0]);
	if (id != 0x626E7254) // bnrT ... watch endianness
		return false;


	// Parse the header
	Height		= *reinterpret_cast<const uint16*>(&headerData[12]);
	Depth		= *reinterpret_cast<const uint16*>(&headerData[14]);
	uint16 flags= *reinterpret_cast<const uint16*>(&headerData[16]);
	Width		= *reinterpret_cast<const uint16*>(&headerData[10]);
	u32 ddsSkew;

	if (flags == 0)		Format = DataFormat::RGBA;
	if (flags & 0x01)	Format = DataFormat::DXT1; ddsSkew = 8;
	if (flags & 0x02)	Format = DataFormat::DXT5; ddsSkew = 16;
	if (flags & 0x03)	Filter = FilterMode::Nearest;
	if (flags & 0x04)	Filter = FilterMode::Trilinear;
	if (flags & 0x05)	Filter = FilterMode::Anisotropic;

	NumFrames	= *reinterpret_cast<const u16*>(&headerData[18]);
	FrameRate	= *reinterpret_cast<const u8*> (&headerData[20]);
	MipMapCount	= *reinterpret_cast<const u8*> (&headerData[21]);


	// Read the actual texture data
	const u8* textureData = static_cast<const u8*>(&headerData[22]);

	u32 mipOffset;
	switch (this->Format)
	{
		case DataFormat::RGBA:
		case DataFormat::BGRA:
			mipOffset = (this->Width * this->Height) * 4;
			break;

		case DataFormat::DXT1:
		case DataFormat::DXT3:
		case DataFormat::DXT5:
			// http://msdn.microsoft.com/en-us/library/windows/desktop/bb943991%28v=vs.85%29.aspx
			mipOffset = (max(1, ((this->Width + 3) / 4)) * max(1, ((this->Height + 3) / 4))) * ddsSkew;
			break;

		default:
			mipOffset = 0;
	}

	// Add all the mips to be bound later
	for (u32 i = 0; i < this->MipMapCount; i++)
	{
		this->Subresources.PushBack(Subresource(textureData, Width * 4));

		// mipOffset will be 0, but we shouldn't be here anyway if it is
		rawData		+= mipOffset;
		mipOffset	/= 4;
	}

	return true;
}