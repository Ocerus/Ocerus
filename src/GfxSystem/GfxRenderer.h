#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "Base.h"
#include "GfxStructures.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

namespace GfxSystem
{
	class GfxRenderer : public Singleton<GfxRenderer> 
	{
	public:
		virtual void Init() = 0;

		virtual bool BeginRendering() = 0;
		
		virtual void EndRendering() = 0;

		virtual void DrawTestQuad() = 0;
	};
}

#endif