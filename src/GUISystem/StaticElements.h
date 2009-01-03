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
		void InitElement( const string & id );
		void SetStaticTextWorker( const string & text, const GfxSystem::Color & color, const string & fontid = "" );
	public:
		StaticText( float32 x, float32 y, const string & id, const string & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const string & fontid = "" );
		StaticText( int32 x, int32 y, const string & id, const string & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const string & fontid = "" );

		void SetColor( const GfxSystem::Color & color );		
		void SetFont( const string & fontid );
		void SetText( const string & text );

		void SetStaticText( float32 x, float32 y, const string & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const string & fontid = "" );
		void SetStaticText( int32 x, int32 y, const string & text, const GfxSystem::Color & color,
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const string & fontid = "" );

		virtual ~StaticText();
	};
}

#endif