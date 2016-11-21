#pragma once
#include "Core\Types.h"

struct FrameInfo
{
	size_t VertsSubmitted;
	size_t TrianglesSubmitted;

	size_t DrawCalls;
	size_t DrawCallsIndexed;
	size_t DrawCallsInstanced;
	size_t DrawCallsIndirect;

	size_t MemoryAllocated;

	size_t StateChanges;
	size_t BufferBinds;
	size_t TextureSwitches;
	size_t ShaderSwitches;
};