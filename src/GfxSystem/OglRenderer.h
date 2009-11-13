#ifndef _OGLRENDERER_H_
#define _OGLRENDERER_H_

#include "Base.h"
#include "Singleton.h"

#include "GfxRenderer.h"

#define gOglRenderer GfxSystem::OglRenderer::GetSingleton()

namespace GfxSystem
{
	class OglRenderer : public GfxRenderer, public Singleton<OglRenderer> 
	{
	public:
		virtual void Init();

		virtual bool BeginRendering();
		
		virtual void EndRendering();

		virtual void DrawTestQuad();
	};
}

#endif