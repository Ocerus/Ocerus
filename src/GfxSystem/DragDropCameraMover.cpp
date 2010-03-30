#include "Common.h"
#include "GfxViewport.h"

#include "Common.h"
#include "DragDropCameraMover.h"

using namespace InputSystem;

// little hack to enable cmZoomModifier to be const and have more constructors with having the value in 1 place
#define ZOOMMOD 1.0f/1200.0f

GfxSystem::DragDropCameraMover::DragDropCameraMover()
:cmZoomModifier(ZOOMMOD)
{
	mRenderTarget = 0;
	mLastPosValid = false;
	mZoomSensitivity = cmZoomModifier;		// default zoom sensitivity (zoom is changed by 10%)
	mMinZoom = 0.1f;						// default minimal zoom
	mMaxZoom = 10.0f;						// default maximal zoom
}

GfxSystem::DragDropCameraMover::DragDropCameraMover( const RenderTargetID renderTarget )
:cmZoomModifier(ZOOMMOD)					// not actually required
{
	mLastPosValid = false;
	mRenderTarget = renderTarget;
	mZoomSensitivity = cmZoomModifier;		// default zoom sensitivity (zoom is changed by 10%)
	mMinZoom = 0.1f;						// default minimal zoom
	mMaxZoom = 10.0f;						// default maximal zoom
}

GfxSystem::DragDropCameraMover::DragDropCameraMover(	const RenderTargetID renderTarget,
														const float32 zoomSensitivity,
														const float32 minZoom,
														const float32 maxZoom	)
:cmZoomModifier(ZOOMMOD)
{
	mLastPosValid = false;
	mRenderTarget = renderTarget;
	mZoomSensitivity = zoomSensitivity * cmZoomModifier;
	mMinZoom = minZoom;
	mMaxZoom = maxZoom;
}

bool GfxSystem::DragDropCameraMover::KeyPressed(const KeyInfo& ke)
{
	OC_UNUSED(ke);
	return false;
}

bool GfxSystem::DragDropCameraMover::KeyReleased( const KeyInfo& ke )
{
	OC_UNUSED(ke);
	return false;
}

bool GfxSystem::DragDropCameraMover::MouseMoved( const MouseInfo& mi )
{

	if (!mLastPosValid && !mi.wheelDelta)		//mouse moved but middle button not pressed and wheel not moved
		return false;							//so nothing to do here ...
		
	if (mi.wheelDelta)							//mouse wheel moved
	{
		float32 zoom = gGfxRenderer.GetRenderTargetCameraZoom(mRenderTarget);

		if (mi.wheelDelta > 0)
			zoom *= 1 + ( mi.wheelDelta * mZoomSensitivity);
		else
			zoom /= 1 + (-mi.wheelDelta * mZoomSensitivity);

		if (zoom > mMaxZoom)
			zoom = mMaxZoom;
		if (zoom < mMinZoom)
			zoom = mMinZoom;

		Vector2 worldCursorPos;
		if (!gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mRenderTarget))
		{
			// we're not in the current viewport
			return false;
		}
		mLastWorldCursorPos = worldCursorPos;	// Sets mLastWorldCursorPos to current worldCursorPos to ensure that
												// the cursor will stay at the same world coord position as before zoom.
												// It is done by the same code that normally does drag'n'drop movement.

		gGfxRenderer.SetRenderTargetCameraZoom(mRenderTarget, zoom);
	}

	Vector2 worldCursorPos;
	if (!gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mRenderTarget))
	{
		// we're not in the current viewport
		return false;
	}

	Vector2 camPos = gGfxRenderer.GetRenderTargetCameraPosition(mRenderTarget);

	camPos += (mLastWorldCursorPos - worldCursorPos);

	gGfxRenderer.SetRenderTargetCameraPosition(mRenderTarget, camPos);

	return true;
}

bool GfxSystem::DragDropCameraMover::MouseButtonPressed( const MouseInfo& mi, const eMouseButton btn )
{

	if (btn != MBTN_MIDDLE)		// intrested only in middle button
		return false;

	Vector2 worldCursorPos;
	if (!gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mRenderTarget))
	{
		// we're not in the current viewport
		mLastPosValid = false;
		return false;
	}

	mLastWorldCursorPos = worldCursorPos;
	mLastPosValid = true;

	return true;
}

bool GfxSystem::DragDropCameraMover::MouseButtonReleased( const MouseInfo& mi, const eMouseButton btn )
{
	if (btn != MBTN_MIDDLE)		// instrested only in middle button
		return false;

	mLastPosValid = false;
	return true;
}