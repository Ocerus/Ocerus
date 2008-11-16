#include "Common.h"
#include "RendererGate.h"
#include "CEGUITextureWrapper.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"
#include "../ResourceSystem/ResourceMgr.h"
#include "../LogSystem/LogMgr.h"
#include <math.h>

namespace GUISystem {

	Quad_info::Quad_info() : dest_rect(0,0,0,0), z(0), texture_rect(0,0,0,0) {
		tex.SetNull();
	}

	Quad_info::Quad_info(GfxSystem::Rect dest_rect, float32 z, GfxSystem::TexturePtr tex, GfxSystem::Rect texture_rect) :
		dest_rect(dest_rect), z(z), tex(tex), texture_rect(texture_rect) {
	}

	RendererGate::RendererGate() : mQueueing(false) {
		d_resourceProvider = new ResourceGate();
		this->d_identifierString = "Black_Hand's wrapper around that disgusting piece of code created by Santhos :PPP";
		this->setQueueingEnabled(false);
	}

	RendererGate::~RendererGate() {
		//ClearProviders();
		// Hardwire: I had to comment this out as it was causing access violation during the shutdown
		//DYN_DELETE d_resourceProvider;
		clearRenderList();
		destroyAllTextures();
	}

	// add's a quad to the list to be rendered
	void RendererGate::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode) {		

		GfxSystem::Rect conv_dest_rect(round(dest_rect.d_left), round(dest_rect.d_top),
			round(dest_rect.getHeight()), round(dest_rect.getWidth()));
		GfxSystem::TexturePtr conv_tex = ((CEGUITextureWrapper*)tex)->getTexture();
		GfxSystem::Rect conv_texture_rect;

		conv_texture_rect.x = (texture_rect.d_left != HUGE_VAL)?round(texture_rect.d_left*texture_rect.getHeight()):0;
		conv_texture_rect.y = (texture_rect.d_top != HUGE_VAL)?round(texture_rect.d_top*texture_rect.getHeight()):0;
		conv_texture_rect.w = (texture_rect.d_right != HUGE_VAL)?round(texture_rect.d_right*conv_tex->GetHeight() - texture_rect.d_left*conv_tex->GetWidth()):conv_tex->GetHeight();
		conv_texture_rect.h = (texture_rect.d_bottom != HUGE_VAL)?round(texture_rect.d_bottom*conv_tex->GetWidth() - texture_rect.d_top*conv_tex->GetWidth()):conv_tex->GetWidth();

		if (mQueueing)
			mQuads.push(Quad_info(conv_dest_rect, z, conv_tex, conv_texture_rect));
		else
			DrawQuad(Quad_info(conv_dest_rect, z, conv_tex, conv_texture_rect));
	}

	// perform final rendering for all queued renderable quads.
	void RendererGate::doRender(void) {
		gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));
		while (!mQuads.empty()) {			
			DrawQuad(mQuads.front());
			mQuads.pop();
		}
	}

	// clear the queue
	void RendererGate::clearRenderList(void) {
		while (!mQuads.empty()) mQuads.pop();
	}

	CEGUI::Texture* RendererGate::createTexture(void) {
		CEGUITextureWrapper * wrap = new CEGUITextureWrapper(this);
		mTextures.insert(wrap);
		
		return wrap;
	}

	CEGUI::Texture* RendererGate::createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		CEGUITextureWrapper * wrap = (CEGUITextureWrapper *)createTexture();
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
		mTextures.clear();
	}

	void RendererGate::DrawQuad(const Quad_info & quad) const {
		gGfxRenderer.DrawImage(quad.tex, quad.texture_rect, quad.dest_rect);
	}
/*
	void RendererGate::ClearProviders() {
		while (!mGivenResourceProviders.empty()) {
			DYN_DELETE mGivenResourceProviders.back();
			mGivenResourceProviders.pop_back();
		}
	}
*/
	void RendererGate::EventResolutionChanged(int x, int y) {
		gGUIMgr.mCegui->fireEvent(EventDisplaySizeChanged, CEGUI::EventArgs());
	}
}
