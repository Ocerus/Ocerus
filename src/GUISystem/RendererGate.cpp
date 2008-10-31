#include "RendererGate.h"
#include "CEGUITextureWrapper.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"
#include "../ResourceSystem/ResourceMgr.h"

namespace GUISystem {

	// add's a quad to the list to be rendered
	void RendererGate::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode) {

				GfxSystem::TexturePtr convertedtex = ((CEGUITextureWrapper*)tex)->getTexture();
				GfxSystem::Rect convertedrect(round(texture_rect.d_left), round(texture_rect.d_top),
					round(texture_rect.getHeight()), round(texture_rect.getWidth()));
								
				gGfxRenderer.DrawImage(convertedtex, convertedrect, 255);
	}

	// perform final rendering for all queued renderable quads.
	void RendererGate::doRender(void) {
	}

	// clear the queue
	void RendererGate::clearRenderList(void) {
	}

	void RendererGate::setQueueingEnabled(bool setting) {
	}

	CEGUI::Texture* RendererGate::createTexture(void) {
		//return new CEGUITextureWrapper(0);
		return 0;
	}

	CEGUI::Texture* RendererGate::createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		CEGUITextureWrapper * wrap = new CEGUITextureWrapper(this); // todle proste chce nakej manazer textur :X
		wrap->loadFromFile(filename, resourceGroup);
		return wrap;
	}

	CEGUI::Texture* RendererGate::createTexture(float size) {
		//return new CEGUITextureWrapper(size);
		return 0;
	}

}