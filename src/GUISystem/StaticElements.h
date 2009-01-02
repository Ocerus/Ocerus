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
		void SetPosition( float32 x, float32 y, uint8 anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP );
		void SetPosition( int32 x, int32 y, uint8 anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP );
		virtual ~StaticElement();
	};

	class StaticText : public StaticElement {
	protected:
		void InitElement();		
	public:
		StaticText( float32 x, float32 y, const std::string & text, const GfxSystem::Color & color,
			uint8 anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const std::string & fontid = "" );
		StaticText( int32 x, int32 y, const std::string & text, const GfxSystem::Color & color,
			uint8 anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const std::string & fontid = "" );

		void SetColor( const GfxSystem::Color & color );		
		void SetFont( const std::string & fontid );
		void SetText( const std::string & text );

		virtual ~StaticText();
	};
}

#endif