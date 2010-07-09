#include "Common.h"
#include "GfxRenderer.h"
#include "GfxSystem/GfxSceneMgr.h"
#include "GfxSystem/Texture.h"
#include "GfxSystem/Mesh.h"
#include "EntitySystem/Components/Sprite.h"
#include "EntitySystem/Components/Model.h"
#include "EntitySystem/Components/Transform.h"

using namespace GfxSystem;

/// Constant which binds pixel and world units together. And image of this size will be 1.0 units big in the world.
const float32 GfxRenderer::PIXELS_PER_WORLD_UNIT = 50;

/// Depth of each layer.
const float32 LAYER_Z_SIZE = 5.0f;


GfxSystem::GfxRenderer::GfxRenderer(): mCurrentRenderTargetID(InvalidRenderTargetID), mIsRendering(false), mSceneMgr(0)
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
	if (BeginRenderingImpl())
	{
		ClearScreen(Color(100,100,100));
		return true;
	}
	else
	{
		return false;
	}
}

void GfxSystem::GfxRenderer::EndRendering()
{
	OC_ASSERT(mIsRendering);
	EndRenderingImpl();
	mIsRendering = false;
	mCurrentRenderTargetID = -1;
}

GfxSystem::RenderTargetID GfxSystem::GfxRenderer::AddRenderTarget( const GfxViewport& viewport, const EntitySystem::EntityHandle camera )
{
	OC_ASSERT(!mIsRendering);

	if (camera.IsValid() && !gEntityMgr.HasEntityComponentOfType(camera, EntityComponents::CT_Camera))
	{
		ocError << "Can't add render target with invalid camera";
		return InvalidRenderTargetID;
	}

	mRenderTargets.push_back(new RenderTarget(viewport, camera));
	RenderTargetID result = mRenderTargets.size()-1;

	ocInfo << "Created new render target " << result;

	return result;
}

bool GfxSystem::GfxRenderer::SetCurrentRenderTarget( const RenderTargetID toSet )
{
	OC_ASSERT(mIsRendering);

	if (toSet < 0 || toSet >= (int32)mRenderTargets.size()) return false;
	RenderTarget* renderTarget = mRenderTargets[toSet];
	if (!renderTarget) return false;

	mCurrentRenderTargetID = toSet;

	SetViewportImpl(&renderTarget->first);

	// set up some default values if the supplied camera is not valid
	Vector2 position(0, 0);
	float32 zoom = 1;
	float32 rotation = 0;

	// try to get the values from the camera
	if (renderTarget->second.IsValid())
	{
		PropertyHolder positionProp = gEntityMgr.GetEntityProperty(renderTarget->second, "Position" );
		PropertyHolder zoomProp = gEntityMgr.GetEntityProperty(renderTarget->second, "Zoom" );
		PropertyHolder rotationProp = gEntityMgr.GetEntityProperty(renderTarget->second, "Rotation" );
		position = positionProp.GetValue<Vector2>();
		zoom = zoomProp.GetValue<float32>();
		rotation = rotationProp.GetValue<float32>();
	}

	SetCameraImpl(position, zoom, rotation);

	return true;
}

bool GfxSystem::GfxRenderer::RemoveRenderTarget( const RenderTargetID toRemove )
{
	OC_ASSERT(!mIsRendering);
	
	if (toRemove < 0 || toRemove >= (int32)mRenderTargets.size()) return false;
	if (!mRenderTargets[toRemove]) return false;
	
	delete mRenderTargets[toRemove];
	mRenderTargets[toRemove] = 0;

	ocInfo << "Deleted render target " << toRemove;
	
	return true;
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
		if (!camera.IsValid())
			return false;

		Vector2 topleftWorld, bottomrightWorld;
		viewport.CalculateWorldBoundaries(topleftWorld, bottomrightWorld);

		// inverse viewport transform
		Vector2 viewportCenter = 0.5f * Vector2((float32)(topleft.x + bottomright.x), (float32)(topleft.y + bottomright.y));
		worldCoords = Vector2((float32)screenCoords.x, (float32)screenCoords.y) - viewportCenter;

		// inverse projection transform
		worldCoords.x = worldCoords.x * (bottomrightWorld.x-topleftWorld.x) / (bottomright.x-topleft.x);
		worldCoords.y = worldCoords.y * (bottomrightWorld.y-topleftWorld.y) / (bottomright.y-topleft.y);

		// inverse camera transform

		worldCoords = GetInverseCameraTranform(camera, worldCoords);

		return true;
	}

	return false;
}

void GfxSystem::GfxRenderer::DrawEntities()
{
	OC_ASSERT(mIsRendering);

	mSceneMgr->DrawVisibleDrawables();
}

void GfxSystem::GfxRenderer::DrawSprite( const EntitySystem::Component* spriteComponent, const EntitySystem::Component* transformComponent ) const
{
	if (spriteComponent->GetType() != EntitySystem::CT_Sprite || transformComponent->GetType() != EntitySystem::CT_Transform)
	{
		ocError << "Invalid components";
		return;
	}

	EntityComponents::Sprite* sprite = (EntityComponents::Sprite*)spriteComponent;
	EntityComponents::Transform* transform = (EntityComponents::Transform*)transformComponent;

	TexturedQuad quad;
	quad.position = transform->GetPosition();
	quad.scale = transform->GetScale();
	quad.angle = transform->GetAngle();
	quad.z = LAYER_Z_SIZE * (float32)transform->GetLayer();
	quad.transparency = sprite->GetTransparency();
	TexturePtr tex = ((TexturePtr)sprite->GetTexture());
	quad.texture = tex->GetTexture();

	if (!sprite->GetFrameSize().IsZero())
	{
		quad.size.Set((float32)sprite->GetFrameSize().x, (float32)sprite->GetFrameSize().y);
		quad.frameSize.x = (float32)sprite->GetFrameSize().x / tex->GetWidth();
		quad.frameSize.y = (float32)sprite->GetFrameSize().y / tex->GetHeight();

		GfxSystem::Point offset;
		offset.x = sprite->GetFrameIndex() * (sprite->GetFrameSize().x + sprite->GetSkipSpace().x);
		offset.y = (offset.x / tex->GetWidth()) * (sprite->GetFrameSize().y  + sprite->GetSkipSpace().y);
		offset.x = offset.x % tex->GetWidth();

		quad.texOffset.x = (float32)offset.x / tex->GetWidth();
		quad.texOffset.y = (float32)offset.y / tex->GetWidth();
	}
	else
	{
		quad.size.Set((float32)tex->GetWidth(), (float32)tex->GetHeight());
		quad.frameSize.Set(1,1);
		quad.texOffset.Set(0,0);
	}

	DrawTexturedQuad(quad);
}

void GfxSystem::GfxRenderer::DrawModel( const EntitySystem::Component* modelComponent, const EntitySystem::Component* transformComponent ) const
{
	if (modelComponent->GetType() != EntitySystem::CT_Model || transformComponent->GetType() != EntitySystem::CT_Transform)
	{
		ocError << "Invalid components";
		return;
	}

	EntityComponents::Model* model = (EntityComponents::Model*)modelComponent;
	EntityComponents::Transform* transform = (EntityComponents::Transform*)transformComponent;

	TexturedMesh mesh;
	mesh.position = transform->GetPosition();
	mesh.scale = MathUtils::Max(transform->GetScale().x, transform->GetScale().y);
	mesh.angle = transform->GetAngle();
	mesh.z = LAYER_Z_SIZE * (float32)transform->GetLayer();
	mesh.yAngle = model->GetYAngle();
	mesh.transparency = model->GetTransparency();
	mesh.mesh = (((MeshPtr)model->GetMesh())->GetMesh());

	DrawTexturedMesh(mesh);
}

void GfxSystem::GfxRenderer::DrawEntity( const EntitySystem::EntityHandle entity ) const
{
	if (gEntityMgr.HasEntityComponentOfType(entity, CT_Sprite))
	{
		DrawSprite(gEntityMgr.GetEntityComponentPtr(entity, EntitySystem::CT_Sprite), gEntityMgr.GetEntityComponentPtr(entity, CT_Transform));
	}
	if (gEntityMgr.HasEntityComponentOfType(entity, CT_Model))
	{
		DrawModel(gEntityMgr.GetEntityComponentPtr(entity, EntitySystem::CT_Model), gEntityMgr.GetEntityComponentPtr(entity, CT_Transform));
	}
}

GfxViewport* GfxSystem::GfxRenderer::GetRenderTargetViewport(const RenderTargetID renderTarget) const
{
	// we have a predefined render target
	if (renderTarget == InvalidRenderTargetID || renderTarget<0 || renderTarget>=(RenderTargetID)mRenderTargets.size() || !mRenderTargets[renderTarget])
	{
		ocError << "Invalid render target";
		return 0;
	}

	return &mRenderTargets[renderTarget]->first;
}

EntitySystem::EntityHandle GfxSystem::GfxRenderer::GetRenderTargetCamera( const RenderTargetID renderTarget ) const
{
	// we have a predefined render target
	if (renderTarget == InvalidRenderTargetID || renderTarget<0 || renderTarget>=(RenderTargetID)mRenderTargets.size() || !mRenderTargets[renderTarget])
	{
		ocError << "Invalid render target";
		return EntitySystem::EntityHandle::Null;
	}

	return mRenderTargets[renderTarget]->second;
}

float32 GfxSystem::GfxRenderer::GetRenderTargetCameraZoom( const RenderTargetID renderTarget ) const
{
	EntitySystem::EntityHandle cameraHandle = GetRenderTargetCamera(renderTarget);
	if (cameraHandle.IsValid())
	{
		return cameraHandle.GetProperty("Zoom").GetValue<float32>();
	}
	return 0;
}

bool GfxSystem::GfxRenderer::SetRenderTargetCameraZoom( const RenderTargetID renderTarget, const float32 newZoom ) const
{
	EntitySystem::EntityHandle cameraHandle = GetRenderTargetCamera(renderTarget);
	if (cameraHandle.IsValid())
	{
		cameraHandle.GetProperty("Zoom").SetValue<float32>(newZoom);
		return true;
	}
	return false;
}

float32 GfxSystem::GfxRenderer::GetRenderTargetCameraRotation( const RenderTargetID renderTarget ) const
{
	EntitySystem::EntityHandle cameraHandle = GetRenderTargetCamera(renderTarget);
	if (cameraHandle.IsValid())
	{
		return cameraHandle.GetProperty("Rotation").GetValue<float32>();
	}
	return 0;
}

Vector2 GfxSystem::GfxRenderer::GetRenderTargetCameraPosition( const RenderTargetID renderTarget ) const
{
	EntitySystem::EntityHandle cameraHandle = GetRenderTargetCamera(renderTarget);
	if (cameraHandle.IsValid())
	{
		return cameraHandle.GetProperty("Position").GetValue<Vector2>();
	}
	return Vector2();
}

bool GfxSystem::GfxRenderer::SetRenderTargetCameraPosition( const RenderTargetID renderTarget, const Vector2 newPosition ) const
{
	EntitySystem::EntityHandle cameraHandle = GetRenderTargetCamera(renderTarget);
	if (cameraHandle.IsValid())
	{
		cameraHandle.GetProperty("Position").SetValue<Vector2>(newPosition);
		return true;
	}
	return false;
}

Vector2 GfxSystem::GfxRenderer::GetInverseCameraTranform( const EntitySystem::EntityHandle& cameraHandle, const Vector2& vec ) const
{
	Vector2 result = vec;
	// inverse camera transform
	result *= 1.0f / cameraHandle.GetProperty("Zoom").GetValue<float32>();
	Matrix22 rotationMatrix(cameraHandle.GetProperty("Rotation").GetValue<float32>());
	result = MathUtils::Multiply(rotationMatrix, result);
	result += cameraHandle.GetProperty("Position").GetValue<Vector2>();
	return result;
}

void GfxSystem::GfxRenderer::CalculateRenderTargetWorldBoundaries( const RenderTargetID renderTargetID, Vector2& min, Vector2& max ) const
{
	GfxViewport* viewport = GetRenderTargetViewport(renderTargetID);
	EntitySystem::EntityHandle cameraHandle = GetRenderTargetCamera(renderTargetID);

	// compute four boundary vecrors in projection space 
	Vector2 topleft, bottomright;
	viewport->CalculateWorldBoundaries(topleft, bottomright);
	
	Vector2 topright, bottomleft;
	topright.x = bottomright.x;
	topright.y = topleft.y;
	bottomleft.x = topleft.x;
	bottomleft.y = bottomright.y;

	// compute four boundary vecrors in world space
	topleft = GetInverseCameraTranform(cameraHandle, topleft);
	bottomright = GetInverseCameraTranform(cameraHandle, bottomright);
	topright = GetInverseCameraTranform(cameraHandle, topright);
	bottomleft = GetInverseCameraTranform(cameraHandle, bottomleft);

	// select mins and maxes
	min.x = MathUtils::Min(topleft.x, MathUtils::Min(bottomright.x, MathUtils::Min( topright.x, bottomleft.x)));
	min.y = MathUtils::Min(topleft.y, MathUtils::Min(bottomright.y, MathUtils::Min( topright.y, bottomleft.y)));
	
	max.x = MathUtils::Max(topleft.x, MathUtils::Max(bottomright.x, MathUtils::Max( topright.x, bottomleft.x)));
	max.y = MathUtils::Max(topleft.y, MathUtils::Max(bottomright.y, MathUtils::Max( topright.y, bottomleft.y)));
}

void GfxSystem::GfxRenderer::DrawGrid( const RenderTargetID renderTargetID ) const
{
	GfxViewport* viewport = GetRenderTargetViewport(renderTargetID);

	if (!viewport->GetGridEnabled())		
		return;
	
	Vector2 min, max;

	// get camera view boundaries in world space
	CalculateRenderTargetWorldBoundaries(renderTargetID, min, max);

	GridInfo grid = viewport->GetGridInfo();

	float32 zoom = GetRenderTargetCameraZoom(renderTargetID);

	bool drawMinors = zoom > grid.minShowMinorsZoom;

	//calculate first vertical x position for minor line
	float32 firstVLineXPos = ceil(min.x / grid.minorCellSize) * grid.minorCellSize;
	
	//calculate first vertical x position for major line
	float32 firstMajorVLineXPos = ceil(min.x / (grid.minorsInMajor * grid.minorCellSize)) * grid.minorCellSize*grid.minorsInMajor;

	//calculate first index for major line
	int32 majorVIndex = (int32) ((firstMajorVLineXPos - firstVLineXPos) / grid.minorCellSize);

	// calculate vertical lines count
	int32 VLineCount = (int32) ceil((max.x - min.x) / grid.minorCellSize);

	// size of cell, depend on whether minor lines should be drawn
	float32 cellSize = grid.minorCellSize;
	
	//recalculations if minors are not drawn
	if (!drawMinors)
	{
		firstVLineXPos = firstMajorVLineXPos;
		VLineCount = (int32) ceil((float32)VLineCount / (float32)grid.minorsInMajor);
		cellSize = cellSize * grid.minorsInMajor;
	}
	

	// draw horizontal lines
	for (int32 i = 0; i < VLineCount; ++i)
	{
		Vector2 line[2];
		line[0].x = line[1].x = firstVLineXPos + i * cellSize;
		line[0].y = min.y;
		line[1].y = max.y;

		if (line[0].x == 0)
		{
			DrawLine(line, grid.axisYColor, 2.0f);
			majorVIndex += grid.minorsInMajor;
		}
		else
		if ((i == majorVIndex) || !drawMinors)
		{
			DrawLine(line, grid.majorColor);
			majorVIndex += grid.minorsInMajor;
		}
		else
		if (drawMinors)
		{
			DrawLine(line, grid.minorColor);
		}
	}

	//calculate first horizontal y position for minor line
	float32 firstHLineYPos = ceil(min.y / grid.minorCellSize) * grid.minorCellSize;
	
	//calculate first horizontal y position for major line
	float32 firstMajorHLineYPos = ceil(min.y / (grid.minorsInMajor * grid.minorCellSize)) * grid.minorCellSize * grid.minorsInMajor;

	//calculate first index for major line
	int32 majorHIndex = (int32) ((firstMajorHLineYPos - firstHLineYPos) / grid.minorCellSize);

	// calculate horizontal lines count
	int32 HLineCount = (int32) ceil((max.y - min.y) / grid.minorCellSize);

	//recalculations if minors are not drawn
	if (!drawMinors)
	{
		firstHLineYPos = firstMajorHLineYPos;
		HLineCount = (int32) ceil((float32)HLineCount / (float32)grid.minorsInMajor);
	}

	// draw horizontal lines
	for (int32 i = 0; i < HLineCount; ++i)
	{
		Vector2 line[2];
		line[0].x = min.x;
		line[1].x = max.x;
		line[0].y = line[1].y = firstHLineYPos + i * cellSize;

		if (line[0].y == 0)
		{
			DrawLine(line, grid.axisXColor, 1.0f);
			majorHIndex += grid.minorsInMajor;
		}
		else
		if ((i == majorHIndex) || !drawMinors)
		{
			DrawLine(line, grid.majorColor);
			majorHIndex += grid.minorsInMajor;
		}
		else
		if (drawMinors)
		{
			DrawLine(line, grid.minorColor);
		}
	}
}

void GfxSystem::GfxRenderer::ClearCurrentRenderTarget( const Color& color ) const
{
	ClearRenderTarget(mCurrentRenderTargetID, color);
}

void GfxSystem::GfxRenderer::ClearRenderTarget( const RenderTargetID target, const Color& color ) const
{
	OC_ASSERT((size_t)target < mRenderTargets.size());
	ClearViewport(mRenderTargets[target]->first, color);
}

bool GfxSystem::GfxRenderer::IsRenderTargetValid( const RenderTargetID toCheck )
{
	return toCheck != InvalidRenderTargetID && (size_t)toCheck < mRenderTargets.size();
}