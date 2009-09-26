#ifndef _STATICELEMENTS_H_
#define _STATICELEMENTS_H_

#include <string>
#include "../GfxSystem/GfxRenderer.h"
#include "CEGUIBase.h"
#include "CEGUIcolour.h"
#include "CEGUIWindow.h"
#include "../Utility/SmartPointer.h"

namespace GUISystem {
	class StaticElement {
	protected:
		CEGUI::Window* mElement;
		static int32 mID;
		void SetSize();
	public:	
		Vector2 GetSize();
		void SetPosition( float32 x, float32 y,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP );
		void SetPosition( int32 x, int32 y,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP );
		virtual ~StaticElement();
	};

	class StaticText : public StaticElement {
	protected:
		void InitElement( const String & id );
		void SetStaticTextWorker( const String & text, const GfxSystem::Color & color, const String & fontid = "" );
	public:
		StaticText( float32 x, float32 y, const String & id, const String & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const String & fontid = "" );
		StaticText( int32 x, int32 y, const String & id, const String & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const String & fontid = "" );

		void SetColor( const GfxSystem::Color & color );		
		void SetFont( const String & fontid );
		void SetText( const String & text );

		void SetStaticText( float32 x, float32 y, const String & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const String & fontid = "" );
		void SetStaticText( int32 x, int32 y, const String & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const String & fontid = "" );

		virtual ~StaticText();
	};
}

#endif