#pragma once
#include "Types.h"
#include "Core\Containers\Array.h"

#include "Renderer\Enums.h"
#include "Renderer\Types.h"

struct File;

// A chunk of data, can be filled prior to creating the resource
// An immutable resource will require this to be done!
struct Subresource
{
	Subresource(void* data, u32 stride = 0) :
		Data	(data),
		Stride	(stride)
	{

	}

	~Subresource()
	{
		// It's not the responsibility of the sub resource to manage whatever is in it!
		// delete Data;
		// Data = nullptr;
	}

//----------------------------------------------------------------

	void*	Data;
	u32		Stride;
};


class RendererBase;

enum class DataFormat
{
	// Typical texture formats
	Unknown,
	RGBA,
	DXT1,
	DXT5,
	DepthStencil,

	// Index buffer type
	Indicies16Bit,
	Indicies32Bit,

	// Input layout component type
	Vector1,
	Vector2,
	Vector3,
	Vector4
};

enum class CPUAccess
{
	Default,
	Write,
	Read,
	ReadWrite,
};

enum class UsagePattern
{
	Default,
	Immutable,
	Static,
	Dynamic,
	Staging,
};

struct Resource
{
	ResourceID GetResourceID() { return resourceID; }

//----------------------------------------------------------------

	Array<Subresource>	Subresources;

	DataFormat			Format;
	CPUAccess			CPUAccessType;
	BindFlags			BindAs;
	UsagePattern		Usage;

//----------------------------------------------------------------

protected:
	// A unique ID should be created and assigned by the renderer when an object is
	// created, this will be used for sorting, indexing, etc later on. When it's no
	// longer needed, the ID should be released
	// UNUSED ATM
	//ResourceID resourceID;

	// Size and stride when it was bound to the pipeline
	// UNUSED ATM
	//u32 boundSize;
	//u32 boundStride;
};