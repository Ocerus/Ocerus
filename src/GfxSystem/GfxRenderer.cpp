#include "Common.h"
#include "GfxRenderer.h"
#include "../GfxSystem/GfxSceneMgr.h"

using namespace GfxSystem;

void GfxRenderer::AddSprite(const Sprite spr)
{
	mSprites.push_back(spr);
}

void GfxRenderer::DrawSprites() const
{
	gGfxSceneMgr.Draw();

	BeginRendering();

	GfxRenderer::SpriteVector::const_iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		//TODO: odecist transformaci kamery, setridit podle textur
		Vector2 pos = (*it).position;
		Vector2 size = (*it).size;
		float32 z = (*it).z;

		SetTexture((*it).texture);

		DrawTexturedQuad(pos, size, z);
	}
	EndRendering();
}