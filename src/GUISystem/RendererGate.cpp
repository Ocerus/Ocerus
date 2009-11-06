#include "Common.h"
#include "RendererGate.h"
#include "CEGUITextureWrapper.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"
#include "../ResourceSystem/ResourceMgr.h"
#include "../LogSystem/LogMgr.h"

namespace GUISystem {

	Quad_info::Quad_info() : dest_rect(0,0,0,0), z(0), texture_rect(0,0,0,0) {
		tex.reset();
	}

	Quad_info::Quad_info(const GfxSystem::Rect& dest_rect, float32 z, GfxSystem::TexturePtr tex, const GfxSystem::Rect& texture_rect,
			const GfxSystem::ColorRect& colors) :
		dest_rect(dest_rect), z(z), tex(tex), texture_rect(texture_rect), colors(colors)
	{
	}

	Quad_info::~Quad_info() {
		tex.reset();
	}

	RendererGate::RendererGate() : mQueueing(false) {
		d_resourceProvider = new ResourceGate();
		this->d_identifierString = "Black_Hand's wrapper around that disgusting piece of code created by Santhos :PPP";
		this->setQueueingEnabled(false);

		gGfxRenderer.AddScreenListener(this);
	}

	RendererGate::~RendererGate() {
		//ClearProviders();
		//delete d_resourceProvider;
		clearRenderList();
		destroyAllTextures();
	}

	/// add's a quad to the list to be rendered
	void RendererGate::addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colors, CEGUI::QuadSplitMode quad_split_mode)
	{
		GfxSystem::Rect conv_dest_rect(MathUtils::Round(dest_rect.d_left), MathUtils::Round(dest_rect.d_top),
			MathUtils::Round(dest_rect.getWidth()), MathUtils::Round(dest_rect.getHeight()));
		GfxSystem::TexturePtr conv_tex = ((CEGUITextureWrapper*)tex)->getTexture();
		GfxSystem::Rect conv_texture_rect;

		conv_texture_rect.x = (texture_rect.d_left != HUGE_VAL)?MathUtils::Round(texture_rect.d_left*conv_tex->GetWidth()):0;
		conv_texture_rect.y = (texture_rect.d_top != HUGE_VAL)?MathUtils::Round(texture_rect.d_top*conv_tex->GetHeight()):0;
		conv_texture_rect.w = (texture_rect.d_right != HUGE_VAL)?MathUtils::Round(texture_rect.d_right*conv_tex->GetWidth() - texture_rect.d_left*conv_tex->GetWidth()):conv_tex->GetWidth();
		conv_texture_rect.h = (texture_rect.d_bottom != HUGE_VAL)?MathUtils::Round(texture_rect.d_bottom*conv_tex->GetHeight() - texture_rect.d_top*conv_tex->GetHeight()):conv_tex->GetHeight();

		Quad_info info(conv_dest_rect, z, conv_tex, conv_texture_rect,
			GfxSystem::ColorRect(colors.d_top_left.getARGB(),
				colors.d_top_right.getARGB(),
				colors.d_bottom_left.getARGB(),
				colors.d_bottom_right.getARGB())
		);

		if (mQueueing)
			mQuads.push_back(info);
		else
			DrawQuad(info);
	}

	// perform final rendering for all queued renderable quads.
	void RendererGate::doRender() {
		vector<Quad_info>::const_iterator iter = mQuads.begin();
		while (iter != mQuads.end()) {
			DrawQuad(*iter);
			++iter;
		}
	}

	// clear the queue
	void RendererGate::clearRenderList(void) {
		mQuads.clear();
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
		OC_ASSERT(!"We are doomed....AGAIN!");
		return 0;
	}

	void RendererGate::destroyTexture(CEGUI::Texture* texture) {
		set<CEGUITextureWrapper*>::iterator found_tex = mTextures.find(static_cast<CEGUITextureWrapper*>(texture));
		if (found_tex != mTextures.end()) {
			delete *found_tex;
			mTextures.erase(found_tex);
		}
	}

	void RendererGate::destroyAllTextures(void) {
		set<CEGUITextureWrapper*>::iterator iter = mTextures.begin();
		while (iter != mTextures.end()) {
			delete *iter;
			++iter;
		}
		mTextures.clear();
	}

	void RendererGate::DrawQuad(const Quad_info & quad) const {
		gGfxRenderer.DrawImage(quad.tex, quad.texture_rect, quad.dest_rect, quad.colors);
	}
/*
	void RendererGate::ClearProviders() {
		while (!mGivenResourceProviders.empty()) {
			delete mGivenResourceProviders.back();
			mGivenResourceProviders.pop_back();
		}
	}
*/
}
