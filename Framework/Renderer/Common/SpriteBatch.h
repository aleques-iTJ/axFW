#pragma once
#include "Core\Math\Matrix.h"
#include "Core\Math\Rect.h"
#include "Core\Containers\Array.h"
#include "Renderer\General\Color.h"

#include "Renderer\Enums.h"

struct BNR_API SB_CBuffer
{
	Matrix proj;
};

//----------------------------------------------------------------

typedef InputLayout::PCTVertex Vertex2D;

struct Batch
{
	Vertex2D*	verts;
	u32			numSprites;
	u32			numVerts;
	u32			numIndicies;
	u32			size;

	Texture*	texture;
};

//----------------------------------------------------------------

static const u32 MAX_INDICIES	= 65532; // Just about enough to fill a 16-bit index buffer
static const u32 MAX_SPRITES	= MAX_INDICIES / 6;
static const u32 MAX_VERTS		= MAX_SPRITES * 4;
static const u32 MAX_SIZE		= MAX_VERTS * sizeof(Vertex2D);

static const u16 VERTS_PER		= 4;
static const u16 INDICIES_PER	= 6;

// This thing is in shambles right now, expect terrors
class BNR_API SpriteBatch
{
public:
	SpriteBatch	();
	~SpriteBatch();

//----------------------------------------------------------------

	bool Create			(Renderer* renderer);
	void Destroy		();

	//void Reserve		(u32 count);

	void SetTexture		(Texture& texture);
	void SetTransform	(Matrix& matrix);

	void Purge			();
	void AddQuad		(const Rect<f32>& pos, const Color& color, const Rect<f32>& uv, f32 z = 1.0f);
	void Draw			();

//----------------------------------------------------------------

private:
	Renderer*		renderer;

	// What we own
	Vertex2D*		verts;
	u16*			indicies;
	Texture*		texture;

	Buffer			vb;
	Buffer			ib;
	Buffer			cb;	
	Shader			shader;
	SamplerState	sampler;	

	// What we're backing up
	// Buffer* lastVB[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	// Buffer* lastIB;

	// Internal stuff
	Array<Batch>	batches;
	u32				batchIndex;
};