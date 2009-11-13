#ifndef _OGLRENDERER_H_
#define _OGLRENDERER_H_

#include "Base.h"
#include "Singleton.h"

#include "GfxRenderer.h"

#define gOGlRenderer GfxSystem::OGlRenderer::GetSingleton()

namespace GfxSystem
{
	class OGlRenderer : public GfxRenderer, public Singleton<OGlRenderer> 
	{
	public:
		virtual void Init();

		virtual bool BeginRendering();
		
		virtual void EndRendering();

		virtual void DrawTestQuad();
	};
}

#endif