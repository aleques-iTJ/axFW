#pragma once
#include "Core\FileSystem\File.h"

struct BNR_API ShaderBase
{
public:
	ShaderBase()
	{

	};

	~ShaderBase()
	{

	};

//----------------------------------------------------------------

protected:
	String	rawShaderText;
	File*	file;
};