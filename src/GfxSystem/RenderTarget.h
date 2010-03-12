/// @file
/// Target of rendering.

#ifndef RenderTarget_h__
#define RenderTarget_h__

namespace GfxSystem
{
	/// Unique identifier of a render target.
	typedef int32 RenderTargetID;

	/// Invalid render target ID.
	static const RenderTargetID InvalidRenderTargetID = -1;

	/// Target of rendering.
	typedef pair<GfxViewport, EntitySystem::EntityHandle> RenderTarget;

}

#endif // RenderTarget_h__