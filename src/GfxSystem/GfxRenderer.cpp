#include "Common.h"
#include "GfxRenderer.h"
#include "../GfxSystem/GfxSceneMgr.h"

using namespace GfxSystem;

GfxSystem::GfxRenderer::GfxRenderer(): mIsRendering(false)
{

}

GfxSystem::GfxRenderer::~GfxRenderer()
{
	for (RenderTargetsVector::iterator it=mRenderTargets.begin(); it!=mRenderTargets.end(); ++it)
	{
		if (*it)
		{
			delete *it;
		}
	}
}

bool GfxSystem::GfxRenderer::BeginRendering()
{
	OC_ASSERT(!mIsRendering);
	mIsRendering = true;
	return BeginRenderingImpl();
}

void GfxSystem::GfxRenderer::EndRendering()
{
	OC_ASSERT(mIsRendering);
	EndRenderingImpl();
	mIsRendering = false;
}

GfxSystem::GfxRenderer::RenderTargetID GfxSystem::GfxRenderer::AddRenderTarget( const GfxViewport& viewport, const EntitySystem::EntityHandle camera )
{
	OC_ASSERT(!mIsRendering);

	if (!gEntityMgr.HasEntityComponentOfType(camera, EntityComponents::CT_Camera))
	{
		ocError << "Can't add render target with invalid camera";
		return InvalidRenderTargetID;
	}

	mRenderTargets.push_back(new ViewportWithCamera(viewport, camera));
	return mRenderTargets.size()-1;
}

bool GfxSystem::GfxRenderer::SetCurrentRenderTarget( const RenderTargetID toSet )
{
	OC_ASSERT(mIsRendering);

	if (toSet < 0 || toSet >= (int32)mRenderTargets.size()) return false;
	ViewportWithCamera* viewportWithCamera = mRenderTargets[toSet];
	if (!viewportWithCamera) return false;

	SetCurrentViewportImpl(viewportWithCamera->first);

	// here we're sure these properties exist since we checked that the camera entity is really a camera
	PropertyHolder position = gEntityMgr.GetEntityProperty(viewportWithCamera->second, "Position" );
	PropertyHolder zoom = gEntityMgr.GetEntityProperty(viewportWithCamera->second, "Zoom" );
	PropertyHolder rotation = gEntityMgr.GetEntityProperty(viewportWithCamera->second, "Rotation" );
	SetCurrentCameraImpl(position.GetValue<Vector2>(), zoom.GetValue<float32>(), rotation.GetValue<float32>());

	return true;
}

bool GfxSystem::GfxRenderer::RemoveRenderTarget( const RenderTargetID toRemove )
{
	OC_ASSERT(!mIsRendering);
	
	if (toRemove < 0 || toRemove >= (int32)mRenderTargets.size()) return false;
	if (!mRenderTargets[toRemove]) return false;
	
	delete mRenderTargets[toRemove];
	mRenderTargets[toRemove] = 0;
	
	return true;
}

void GfxRenderer::AddSprite(const Sprite spr)
{
	mSprites.push_back(spr);
}

void GfxRenderer::DrawSprites()
{
	OC_ASSERT(mIsRendering);

	gGfxSceneMgr.Draw();

	SpriteVector::const_iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		//TODO: setridit podle textur
		Vector2 pos = (*it).position;
		Vector2 size = (*it).size;
		float32 z = (*it).z;
		float32 transp = (*it).transparency;

		SetTexture((*it).texture);

		DrawTexturedQuad(pos, size, z, transp);
	}

	ResetSprites();
}