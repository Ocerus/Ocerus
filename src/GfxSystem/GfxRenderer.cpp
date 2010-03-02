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

	SetViewportImpl(viewportWithCamera->first);

	// here we're sure these properties exist since we checked that the camera entity is really a camera
	PropertyHolder position = gEntityMgr.GetEntityProperty(viewportWithCamera->second, "Position" );
	PropertyHolder zoom = gEntityMgr.GetEntityProperty(viewportWithCamera->second, "Zoom" );
	PropertyHolder rotation = gEntityMgr.GetEntityProperty(viewportWithCamera->second, "Rotation" );
	SetCameraImpl(position.GetValue<Vector2>(), zoom.GetValue<float32>(), rotation.GetValue<float32>());

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
		/*Vector2 pos = (*it).position;
		Vector2 size = (*it).size;
		float32 z = (*it).z;
		float32 transp = (*it).transparency;*/

		SetTexture((*it).texture);

		DrawSprite(*it);
	}

	ResetSprites();
}

bool GfxSystem::GfxRenderer::ConvertScreenToWorldCoords( const Point& screenCoords, Vector2& worldCoords ) const
{
	// find the render target the pixel is in
	for (RenderTargetsVector::const_iterator it=mRenderTargets.begin(); it!=mRenderTargets.end(); ++it)
	{
		if (!*it) continue;

		GfxViewport& viewport = (*it)->first;
		Point topleft, bottomright;
		CalculateViewportScreenBoundaries(viewport, topleft, bottomright);

		if (topleft.x <= screenCoords.x && screenCoords.x <= bottomright.x && topleft.y <= screenCoords.y && screenCoords.y <= bottomright.y)
		{
			EntitySystem::EntityHandle camera = (*it)->second;
			Vector2 topleftWorld, bottomrightWorld;
			CalculateViewportWorldBoundaries(viewport, topleftWorld, bottomrightWorld);

			// inverse viewport transform
			worldCoords = Vector2((float32)screenCoords.x, (float32)screenCoords.y) - Vector2((float32)topleft.x, (float32)topleft.y);
			
			// inverse projection transform
			worldCoords.x = worldCoords.x * (bottomrightWorld.x-topleftWorld.x) / (bottomright.x-topleft.x);
			worldCoords.y = worldCoords.y * (bottomrightWorld.y-topleftWorld.y) / (bottomright.y-topleft.y);

			// inverse camera transform
			worldCoords += camera.GetProperty("Position").GetValue<Vector2>();
			worldCoords *= 1.0f / camera.GetProperty("Zoom").GetValue<float32>();
			Matrix22 rotationMatrix(-camera.GetProperty("Rotation").GetValue<float32>());
			worldCoords = MathUtils::Multiply(rotationMatrix, worldCoords);

			return true;
		}
	}

	return false;
}

void GfxSystem::GfxRenderer::CalculateViewportWorldBoundaries( const GfxViewport& viewport, Vector2& topleft, Vector2& bottomright ) const
{
	if (viewport.relative)
	{
		// size of objects depends on resolution
		topleft.x = -smOrthoSizeX * viewport.size.x;
		bottomright.x = smOrthoSizeX * viewport.size.x;
		topleft.y = -smOrthoSizeY * viewport.size.y;
		bottomright.y = smOrthoSizeY * viewport.size.y;
	}
	else
	{
		// size of objects doesn't depend on resolution
		int32 resx = gGfxWindow.GetResolutionWidth();
		int32 resy = gGfxWindow.GetResolutionHeight();
		topleft.x = -resx * viewport.size.x / 2;
		bottomright.x = resx * viewport.size.x / 2;
		topleft.y = -resy * viewport.size.y / 2;
		bottomright.y = resy * viewport.size.y / 2;
	}
}

void GfxSystem::GfxRenderer::CalculateViewportScreenBoundaries( const GfxViewport& viewport, Point& topleft, Point& bottomright ) const
{
	int32 resx = gGfxWindow.GetResolutionWidth();
	int32 resy = gGfxWindow.GetResolutionHeight();

	topleft.x = (int32)(viewport.position.x * resx);
	topleft.y = (int32)(viewport.position.y * resy);
	bottomright.x = topleft.x + (int32)(viewport.size.x * resx);
	bottomright.y = topleft.y + (int32)(viewport.size.y * resy);
}