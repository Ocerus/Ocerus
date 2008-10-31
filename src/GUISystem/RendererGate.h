#ifndef _RENDERERGATE_H_
#define _RENDERERGATE_H_

#include "../Utility/Settings.h"
#include "CEGUIRenderer.h"
#include <queue>

namespace GUISystem {
	class RendererGate : public CEGUI::Renderer
	{
	public:
		// add's a quad to the list to be rendered - Pokud Santhos neda tak bude jen vykreslovat
		virtual	void addQuad(const CEGUI::Rect& dest_rect, float z, const CEGUI::Texture* tex,
			const CEGUI::Rect& texture_rect, const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode);

		// perform final rendering for all queued renderable quads. - Pokud Santhos da tak addQuad bude opravdu jen
		// pridavat a todle jen vykreslovat
		virtual	void doRender(void);

		// clear the queue
		virtual	void clearRenderList(void);

		virtual void setQueueingEnabled(bool setting);

		virtual	CEGUI::Texture* createTexture(void);

		virtual	CEGUI::Texture* createTexture(const CEGUI::String& filename, const CEGUI::String& resourceGroup);

		virtual	CEGUI::Texture* createTexture(float size);

		virtual	void destroyTexture(CEGUI::Texture* texture);

		virtual void destroyAllTextures(void);

		virtual bool isQueueingEnabled(void) const;

		virtual float getWidth(void) const;

		virtual float getHeight(void) const;

		// Return the size of the display in pixels
		virtual CEGUI::Size getSize(void) const;

		// Return a Rect describing the screen
		virtual CEGUI::Rect getRect(void) const;

		virtual	CEGUI::uint getMaxTextureSize(void) const;

		// Return the horizontal display resolution dpi
		virtual	CEGUI::uint getHorzScreenDPI(void) const;

		// Return the vertical display resolution dpi
		virtual	CEGUI::uint getVertScreenDPI(void) const;

		const CEGUI::String& getIdentifierString() const;

		virtual CEGUI::ResourceProvider* createResourceProvider(void);	
	};
}

#endif