#pragma once
#include "Renderer\Frontend\SpriteBatch.h"

#include "Renderer\General\Viewport.h"
#include "Renderer\General\Color.h"

// Fix
#include "Core\System\SystemW32.h"

SpriteBatch::SpriteBatch()
{
	
}

SpriteBatch::~SpriteBatch()
{
	Destroy();
}

//----------------------------------------------------------------

bool SpriteBatch::Create(Renderer* renderer)
{
	if (renderer == nullptr)
		return false;

	this->renderer = renderer;

	// Fill the index buffer ahead of time, so we don't have to calculate for it later
	u16* temp = new u16[MAX_INDICIES * 6];
	for (u16 i = 0; i < MAX_SPRITES; i++)
	{
		u16 index = i * INDICIES_PER;	// 65532
		u16 dorse = i * VERTS_PER;		// 43688

		temp[index + 0] = 0 + dorse;	// bl
		temp[index + 1] = 1 + dorse;	// tl
		temp[index + 2] = 2 + dorse;	// br
		temp[index + 3] = 2 + dorse;	// br
		temp[index + 4] = 1 + dorse;	// tl
		temp[index + 5] = 3 + dorse;	// tr
	}

	// Some simple setup for 2d drawing, grab the viewport so we know the pixel height / width
	Viewport vp;
	this->renderer->GetViewport(vp);
	auto mat = Matrix::Ortho2D(vp.Width, vp.Height, 0.1f, 100.f);

	// Create the buffers... only the vertex buffer will need to change
	memset(&vb, 0, sizeof(vb));
	memset(&ib, 0, sizeof(ib));
	memset(&cb, 0, sizeof(cb));
	vb.Size		= MAX_VERTS * sizeof(Vertex2D);
	vb.Usage	= UsagePattern::Dynamic;
	vb.BindAs	= BindFlags::VertexBuffer;
	vb.RWAccess	= ReadWriteAccess::WriteAccess;
	vb.Stride	= sizeof(Vertex2D);

	ib.Size		= MAX_INDICIES * 2;	// 2 bytes per index
	ib.Usage	= UsagePattern::Immutable;	// The index buffer should exist from start to finish
	ib.BindAs	= BindFlags::IndexBuffer;
	ib.Format	= Format::Indicies16Bit;
	Subresource ibSR(temp);
	ib.Subresources.Add(ibSR);

	cb.Size		= sizeof(SB_CBuffer);	// This should maybe be static?
	cb.Usage	= UsagePattern::Immutable;		// Projection CAN (should) change if the renderer resizes
	cb.BindAs	= BindFlags::ConstantBuffer;
	Subresource cbSR(&mat);
	cb.Subresources.Add(cbSR);

	this->renderer->CreateBuffer(vb);
	this->renderer->CreateBuffer(ib);
	this->renderer->CreateBuffer(cb);
	vb.Activate(0);
	ib.Activate(0);
	cb.Activate(0);

	// Everything is uploaded at this point
	delete[] temp;
	temp = nullptr;

	String shaderPath = System::GetExeDirectory();
	shaderPath.Append("BasicSprite_PCT.txt");
	shader.ShaderType	= ShaderType(ShaderType::VertexShader | ShaderType::PixelShader);
	shader.InputType	= InputLayout::Type::PositionColorTexcoord;
	shader.LoadFromFile(shaderPath);
	this->renderer->CreateShader(shader);
	shader.Activate();

	// Set up the sampler
	sampler.SetDefaults();
	sampler.Filter = FilterMode::Nearest;
	this->renderer->CreateSamplerState(sampler);
	sampler.Activate(0);

	// Just make sure we're set to the correct primitive type
	this->renderer->SetPrimitiveType(PrimitiveType::TriangleList);

	// Finally, set up some basic batch information
	batches.PushBack();
	batches[0].verts		= new Vertex2D[MAX_VERTS];
	batches[0].numVerts		= 0;
	batches[0].numSprites	= 0;
	batches[0].numIndicies	= 0;
	batches[0].size			= 0;

	batchIndex = 0;

	return true;
}

void SpriteBatch::Destroy()
{
	for (u32 i = 0; i < batches.GetSize(); i++)
		delete[] batches[i].verts;
}

//----------------------------------------------------------------

void SpriteBatch::SetTexture(Texture& texture)
{
	this->texture = &texture;
	texture.Activate(0);		// This is a hazard...
}

void SpriteBatch::SetTransform(Matrix& matrix)
{
	matrix = matrix;
}

//----------------------------------------------------------------

void SpriteBatch::Purge()
{
	for (u32 i = 0; i < batches.GetSize(); i++)
	{
		// The array of verts in the batch still holds all the old data!
		// The space is still allocated!
		batches[i].numVerts		= 0;
		batches[i].numSprites	= 0;
		batches[i].numIndicies	= 0;
		batches[i].size			= 0;
	}

	batchIndex = 0;
}

void SpriteBatch::AddQuad(const Rect<f32>& pos, const Color& color, const Rect<f32>& uv, f32 z)
{
	u32			index = VERTS_PER * batches[batchIndex].numSprites;
	Vertex2D*	vbPtr = batches[batchIndex].verts;

	// + 0
	vbPtr[index].x = pos.X;			// Bottom left
	vbPtr[index].y = pos.Height;
	vbPtr[index].z = z;
	vbPtr[index].r = color.Red;
	vbPtr[index].g = color.Green;
	vbPtr[index].b = color.Blue;
	vbPtr[index].u = uv.X;
	vbPtr[index].v = uv.Height;
	index++;

	// + 1
	vbPtr[index].x = pos.X;			// Top left
	vbPtr[index].y = pos.Y;
	vbPtr[index].z = z;
	vbPtr[index].r = color.Red;
	vbPtr[index].g = color.Green;
	vbPtr[index].b = color.Blue;
	vbPtr[index].u = uv.X;
	vbPtr[index].v = uv.Y;
	index++;

	// + 2
	vbPtr[index].x = pos.Width;		// Bottom right
	vbPtr[index].y = pos.Height;
	vbPtr[index].z = z;
	vbPtr[index].r = color.Red;
	vbPtr[index].g = color.Green;
	vbPtr[index].b = color.Blue;
	vbPtr[index].u = uv.Width;
	vbPtr[index].v = uv.Height;
	index++;

	// + 3
	vbPtr[index].x = pos.Width;		// Top right
	vbPtr[index].y = pos.Y;
	vbPtr[index].z = z;
	vbPtr[index].r = color.Red;
	vbPtr[index].g = color.Green;
	vbPtr[index].b = color.Blue;
	vbPtr[index].u = uv.Width;
	vbPtr[index].v = uv.Y;
	index++;

	// Keep tabs on some basic information, it'll be needed in a bit
	batches[batchIndex].numSprites	+= 1;
	batches[batchIndex].numVerts	+= 4;
	batches[batchIndex].numIndicies	+= 6;
	batches[batchIndex].size		 = batches[batchIndex].numVerts * sizeof(Vertex2D);

	if (batches[batchIndex].numSprites >= MAX_SPRITES)
	{
		// We've overflowed and have no room allocated need to start a new batch
		batchIndex++;

		if (batchIndex >= batches.GetSize())
		{
			batches.PushBack();
			batches[batchIndex].verts		= new Vertex2D[MAX_VERTS];
			batches[batchIndex].numVerts	= 0;
			batches[batchIndex].numSprites	= 0;
			batches[batchIndex].numIndicies = 0;
			batches[batchIndex].size		= 0;
		}
	}
}

void SpriteBatch::Draw()
{
	// Doesn't take textures into account yet...
	for (u32 i = 0; i < batches.GetSize(); i++)
	{
		renderer->MapThenUnmap(vb, 0, D3D11_MAP_WRITE_DISCARD, &batches[i].verts[0], batches[i].size);
		renderer->DrawIndexed(batches[i].numIndicies, 0, 0);
	}

	// WHAT
	/*for (s32 i = 0; i < batches.size(); i++)
	{
		Vertex2D*	data	= nullptr;
		u32			offset	= (i & 1) ? MAX_VERTS : 0;

		context->Map(vb, 0, D3D11_MAP_WRITE_NO_OVERWRITE, (void**) &data);
		memcpy(data + offset, batches[i].verts, batches[i].size);
		context->Unmap(vb, 0);

		context->DrawIndexed(batches[i].numIndicies, 0, offset);
	}*/
}