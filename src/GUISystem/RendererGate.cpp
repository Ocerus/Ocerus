#include "RendererGate.h"
#include "CEGUITextureWrapper.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"
#include "../ResourceSystem/ResourceMgr.h"

namespace GUISystem {

	Quad_info::Quad_info() : dest_rect(0,0,0,0), z(0), texture_rect(0,0,0,0) {
		tex.SetNull();
	}

	Quad_info::Quad_info(GfxSystem::Rect dest_rect, float32 z, GfxSystem::TexturePtr tex, GfxSystem::Rect texture_rect) :
		dest_rect(dest_rect), z(z), tex(tex), texture_rect(texture_rect) {
	}

	RendererGate::RendererGate() : mQueueing(false) {
		this->d_identifierString = "Black_Hand's wrapper around that disgusting piece of code created by Santhos :PPP";
	}

	RendererGate::~RendererGate() {
		ClearProviders();
		clearRenderList();
		destroyAllTextures();
	}

	// add's a quad to the list to be rendered
	void RendererGate::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode) {		

		GfxSystem::Rect conv_dest_rect(round(dest_rect.d_left), round(dest_rect.d_top),
			round(dest_rect.getHeight()), round(dest_rect.getWidth()));
		GfxSystem::TexturePtr conv_tex = ((CEGUITextureWrapper*)tex)->getTexture();
		GfxSystem::Rect conv_texture_rect(round(texture_rect.d_left), round(texture_rect.d_top),
			round(texture_rect.getHeight()), round(texture_rect.getWidth()));

		if (mQueueing)
			mQuads.push(Quad_info(conv_dest_rect, z, conv_tex, conv_texture_rect));
		else
			DrawQuad(Quad_info(conv_dest_rect, z, conv_tex, conv_texture_rect));
	}

	// perform final rendering for all queued renderable quads.
	void RendererGate::doRender(void) {
		Quad_info q;
		while (!mQuads.empty()) {
			DrawQuad(q);
			mQuads.pop();
		}
	}

	// clear the queue
	void RendererGate::clearRenderList(void) {
		mQuads.pop();
	}

	CEGUI::Texture* RendererGate::createTexture(void) {
		CEGUITextureWrapper * wrap = new CEGUITextureWrapper(this);
		mTextures.insert(wrap);
		
		return wrap;
	}

	CEGUI::Texture* RendererGate::createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		CEGUI::Texture * wrap = createTexture();
		wrap->loadFromFile(filename, resourceGroup);
		return wrap;
	}

	CEGUI::Texture* RendererGate::createTexture(float size) {
		assert(!"We are doomed....AGAIN!");
		return 0;
	}

	void RendererGate::destroyTexture(CEGUI::Texture* texture) {
		std::set<CEGUITextureWrapper*>::iterator found_tex = mTextures.find(static_cast<CEGUITextureWrapper*>(texture));
		if (found_tex != mTextures.end()) {
			DYN_DELETE *found_tex;
			mTextures.erase(found_tex);
		}
	}

	void RendererGate::destroyAllTextures(void) {
		std::set<CEGUITextureWrapper*>::iterator iter = mTextures.begin();
		while (iter != mTextures.end()) {
			DYN_DELETE *iter;
			++iter;
		}
	}

	void RendererGate::DrawQuad(const Quad_info & quad) const {
		gGfxRenderer.DrawImage(quad.tex, quad.texture_rect, quad.dest_rect);
	}

	void RendererGate::ClearProviders() {
		while (!mGivenResourceProviders.empty()) {
			DYN_DELETE mGivenResourceProviders.back();
			mGivenResourceProviders.pop_back();
		}
	}

	void RendererGate::EventResolutionChanged(int x, int y) {
		gGUIMgr.mCegui->fireEvent(EventDisplaySizeChanged, CEGUI::EventArgs());
	}
}