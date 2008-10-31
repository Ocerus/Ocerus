#include "RendererGate.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"

namespace GUISystem {

	// add's a quad to the list to be rendered
	void RendererGate::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode) {

				GfxSystem::TexturePtr convertedtex = ((CEGUITextureWrapper*)tex)->getTexture();
				GfxSystem::Rect convertedrect;
				convertedrect.x = texture_rect.d_left;
				convertedrect.y = texture_rect.d_top;
				convertedrect.h = texture_rect.getHeight();
				convertedrect.w = texture_rect.getWidth();
								
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
	}

	CEGUI::Texture* RendererGate::createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
	}

	CEGUI::Texture* RendererGate::createTexture(float size) {
	}

}