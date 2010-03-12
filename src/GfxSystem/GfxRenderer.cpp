#include "Common.h"
#include "GfxRenderer.h"
#include "GfxSystem/GfxSceneMgr.h"
#include "GfxSystem/Texture.h"

using namespace GfxSystem;

GfxSystem::GfxRenderer::GfxRenderer(): mIsRendering(false), mSceneMgr(0)
{
	mSceneMgr = new GfxSceneMgr();
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
	if (mSceneMgr)
	{
		delete mSceneMgr;
	}
}

bool GfxSystem::GfxRenderer::BeginRendering()
{
	OC_ASSERT(!mIsRendering);
	OC_ASSERT(mSceneMgr);
	mIsRendering = true;
	return BeginRenderingImpl();
}

void GfxSystem::GfxRenderer::EndRendering()
{
	OC_ASSERT(mIsRendering);
	EndRenderingImpl();
	mIsRendering = false;
}

GfxSystem::RenderTargetID GfxSystem::GfxRenderer::AddRenderTarget( const GfxViewport& viewport, const EntitySystem::EntityHandle camera )
{
	OC_ASSERT(!mIsRendering);

	if (!gEntityMgr.HasEntityComponentOfType(camera, EntityComponents::CT_Camera))
	{
		ocError << "Can't add render target with invalid camera";
		return InvalidRenderTargetID;
	}

	mRenderTargets.push_back(new RenderTarget(viewport, camera));
	return mRenderTargets.size()-1;
}

bool GfxSystem::GfxRenderer::SetCurrentRenderTarget( const RenderTargetID toSet )
{
	OC_ASSERT(mIsRendering);

	if (toSet < 0 || toSet >= (int32)mRenderTargets.size()) return false;
	RenderTarget* renderTarget = mRenderTargets[toSet];
	if (!renderTarget) return false;

	SetViewportImpl(&renderTarget->first);

	// here we're sure these properties exist since we checked that the camera entity is really a camera
	PropertyHolder position = gEntityMgr.GetEntityProperty(renderTarget->second, "Position" );
	PropertyHolder zoom = gEntityMgr.GetEntityProperty(renderTarget->second, "Zoom" );
	PropertyHolder rotation = gEntityMgr.GetEntityProperty(renderTarget->second, "Rotation" );
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

void GfxRenderer::QueueTexturedQuad(const TexturedQuad spr)
{
	mQuads.push_back(spr);
}

void GfxRenderer::ProcessTexturedQuads()
{
	OC_ASSERT(mIsRendering);

	QuadVector::const_iterator it;
	for(it = mQuads.begin(); it != mQuads.end(); ++it)
	{
		//TODO: setridit podle textur
		DrawTexturedQuad(*it);
	}

	mQuads.clear();
}

bool GfxSystem::GfxRenderer::ConvertScreenToWorldCoords( const Point& screenCoords, Vector2& worldCoords, const RenderTargetID renderTargetID ) const
{
	// we have a predefined render target
	if (renderTargetID != InvalidRenderTargetID)
	{
		if (renderTargetID<0 || renderTargetID>=(RenderTargetID)mRenderTargets.size())
		{
			ocError << "Invalid render target";
			return false;
		}
		RenderTarget* renderTarget = mRenderTargets[renderTargetID];
		if (!renderTarget)
		{
			ocError << "Invalid render target";
			return false;
		}
		return ConvertScreenToWorldCoords(screenCoords, worldCoords, *renderTarget);
	}

	// find the render target the pixel is in
	for (RenderTargetsVector::const_iterator it=mRenderTargets.begin(); it!=mRenderTargets.end(); ++it)
	{
		if (!*it) continue;
		if ((*it)->first.AttachedToTexture()) continue;

		if (ConvertScreenToWorldCoords(screenCoords, worldCoords, **it)) return true;
	}

	return false;
}

bool GfxSystem::GfxRenderer::ConvertScreenToWorldCoords( const Point& screenCoords, Vector2& worldCoords, const RenderTarget& renderTarget ) const
{
	const GfxViewport& viewport = renderTarget.first;
	
	Point topleft, bottomright;
	viewport.CalculateScreenBoundaries(topleft, bottomright);

	if (topleft.x <= screenCoords.x && screenCoords.x <= bottomright.x && topleft.y <= screenCoords.y && screenCoords.y <= bottomright.y)
	{
		const EntitySystem::EntityHandle& camera = renderTarget.second;
		Vector2 topleftWorld, bottomrightWorld;
		viewport.CalculateWorldBoundaries(topleftWorld, bottomrightWorld);

		// inverse viewport transform
		Vector2 viewportCenter = 0.5f * Vector2((float32)(topleft.x + bottomright.x), (float32)(topleft.y + bottomright.y));
		worldCoords = Vector2((float32)screenCoords.x, (float32)screenCoords.y) - viewportCenter;

		// inverse projection transform
		worldCoords.x = worldCoords.x * (bottomrightWorld.x-topleftWorld.x) / (bottomright.x-topleft.x);
		worldCoords.y = worldCoords.y * (bottomrightWorld.y-topleftWorld.y) / (bottomright.y-topleft.y);

		// inverse camera transform
		worldCoords += camera.GetProperty("Position").GetValue<Vector2>();
		worldCoords *= 1.0f / camera.GetProperty("Zoom").GetValue<float32>();
		Matrix22 rotationMatrix(camera.GetProperty("Rotation").GetValue<float32>());
		worldCoords = MathUtils::Multiply(rotationMatrix, worldCoords);

		return true;
	}

	return false;
}

void GfxSystem::GfxRenderer::DrawEntities()
{
	OC_ASSERT(mIsRendering);

	mSceneMgr->DrawVisibleSprites();
}

void GfxSystem::GfxRenderer::DrawSprite( const EntitySystem::Component* sprite, const EntitySystem::Component* transform ) const
{
	TexturedQuad quad;
	quad.position = transform->GetProperty("Position").GetValue<Vector2>();
	quad.scale = transform->GetProperty("Scale").GetValue<Vector2>();
	quad.angle = transform->GetProperty("Angle").GetValue<float32>();
	quad.z = (float32)transform->GetProperty("Depth").GetValue<int32>();
	quad.size = sprite->GetProperty("Size").GetValue<Vector2>();
	quad.texture = ((TexturePtr)sprite->GetProperty("Texture").GetValue<ResourceSystem::ResourcePtr>())->GetTexture();
	quad.transparency = sprite->GetProperty("Transparency").GetValue<float32>();

	DrawTexturedQuad(quad);
}

void GfxSystem::GfxRenderer::DrawEntity( const EntitySystem::EntityHandle entity ) const
{
	DrawSprite(gEntityMgr.GetEntityComponent(gEntityMgr.FindFirstEntity("Visual"), EntitySystem::CT_Sprite), gEntityMgr.GetEntityComponent(gEntityMgr.FindFirstEntity("Visual"), CT_Transform));
}