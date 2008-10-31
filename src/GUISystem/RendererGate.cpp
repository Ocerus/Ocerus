#include "RendererGate.h"

namespace GUISystem {

	// add's a quad to the list to be rendered
	virtual	void RendererGate::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode) {


	}

	// perform final rendering for all queued renderable quads.
	virtual	void RendererGate::doRender(void) {
	}

	// clear the queue
	virtual	void RendererGate::clearRenderList(void) {
	}

}