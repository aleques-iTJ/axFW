#pragma once
#include "Types.h"
#include "Renderer\Common\Viewport.h"
#include "Renderer\Common\FrameInfo.h"


// Represents a device command list
class CommandList
{
public:
	// Draw calls
	void Draw		(uint32 vertexCount, uint32 offset = 0);
	void DrawIndexed(uint32 indexCount, uint32 indexOffset = 0, uint32 vertexOffset = 0);

	//virtual void DrawInstanced;

	// Memory
	void Map(const ResourceBase& resource, u32 index, u32 flags, void** ptr)			const;
	void Unmap(const ResourceBase& resource, u32 index = 0)								const;
	void MapThenUnmap(const ResourceBase& resource, u32 index, u32 flags, void* ptr, u32 size)	const; // Just convenience
	void UpdateSubresource(const ResourceBase& resource, u32 index, void* ptr)						const;

	// Other
	void GetViewport(Viewport& viewport) const;
	void SetViewport(const Viewport& viewport);
	void SetPrimitiveType(PrimitiveType primitiveType);

private:

};