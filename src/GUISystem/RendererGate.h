#ifndef _RENDERERGATE_H_
#define _RENDERERGATE_H_

#include "../Utility/Settings.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/IScreenListener.h"
#include "CEGUIRenderer.h"
#include "ResourceGate.h"

namespace GUISystem {

	class CEGUITextureWrapper;
	class GUIMgr;

	struct Quad_info {
		GfxSystem::Rect dest_rect;
		float32 z;
		GfxSystem::TexturePtr tex;
		GfxSystem::Rect texture_rect;
		GfxSystem::ColorRect colors;

		Quad_info();
		Quad_info(const GfxSystem::Rect& dest_rect, float32 z, GfxSystem::TexturePtr tex, const GfxSystem::Rect& texture_rect,
			const GfxSystem::ColorRect& colors);
		virtual ~Quad_info();
	};

	class RendererGate : public CEGUI::Renderer, GfxSystem::IScreenListener
	{
	public:
		RendererGate();
		~RendererGate();

		// add's a quad to the list to be rendered - Pokud Santhos neda tak bude jen vykreslovat
		virtual	void addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colors, CEGUI::QuadSplitMode quad_split_mode);

		// perform final rendering for all queued renderable quads. - Pokud Santhos da tak addQuad bude opravdu jen
		// pridavat a todle jen vykreslovat
		virtual	void doRender(void);

		// clear the queue
		virtual	void clearRenderList(void);

		inline virtual void setQueueingEnabled(bool setting) { mQueueing = setting; }

		virtual	CEGUI::Texture* createTexture(void);

		virtual	CEGUI::Texture* createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup);

		virtual	CEGUI::Texture* createTexture(float size);

		virtual	void destroyTexture(CEGUI::Texture* texture);

		virtual void destroyAllTextures(void);

		inline virtual bool isQueueingEnabled(void) const { return mQueueing; }

		inline virtual float getWidth(void) const { return (float)gGfxRenderer.GetResolution().x; };

		inline virtual float getHeight(void) const { return (float)gGfxRenderer.GetResolution().y; };

		// Return the size of the display in pixels
		inline virtual CEGUI::Size getSize(void) const {
			GfxSystem::Point pt = gGfxRenderer.GetResolution();
			return CEGUI::Size((float)pt.x, (float)pt.y);
		}

		// Return a Rect describing the screen
		inline virtual CEGUI::Rect getRect(void) const {
			return CEGUI::Rect(0.0f, 0.0f, getWidth(), getHeight());
		}

		inline virtual CEGUI::uint getMaxTextureSize(void) const { return UINT_MAX; }

		// Return the horizontal display resolution dpi; 96 is from directx renderer headers
		inline virtual CEGUI::uint getHorzScreenDPI(void) const { return 96; }

		// Return the vertical display resolution dpi; 96 is from directx renderer headers
		inline virtual CEGUI::uint getVertScreenDPI(void) const { return 96; }

		inline virtual CEGUI::ResourceProvider* createResourceProvider(void) {
			if (!d_resourceProvider)
				d_resourceProvider = DYN_NEW ResourceGate();
			return d_resourceProvider;
		}

		inline virtual void ResolutionChanged(int x, int y) {
			gGUIMgr.mCegui->fireEvent(EventDisplaySizeChanged, CEGUI::EventArgs());
		}

	protected:
		virtual void DrawQuad(const Quad_info & quad) const;

		Set<CEGUITextureWrapper*> mTextures;
		Vector<Quad_info> mQuads;

		//void ClearProviders();

		bool mQueueing;		
	};
}

#endif