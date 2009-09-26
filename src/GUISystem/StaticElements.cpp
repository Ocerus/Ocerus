#include "Common.h"
#include "StaticElements.h"
#include "CEGUIBase.h"
#include "CEGUIWindowManager.h"
#include "CEGUIFontManager.h"

//TODO std pryc
#include <sstream>
#include <iomanip>

namespace GUISystem {
	int32 StaticElement::mID = 0;

/*
enum eAnchor {
	ANCHOR_VCENTER=1<<0,
	ANCHOR_TOP=1<<1,
	ANCHOR_BOTTOM=1<<2,
	ANCHOR_LEFT=1<<3,
	ANCHOR_RIGHT=1<<4,
	ANCHOR_HCENTER=1<<5
};
*/

	void StaticElement::SetPosition( float32 x, float32 y,
			uint8 text_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			uint8 screen_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/ )
	{
		SetSize();
		Vector2 offset = GetSize();
		if ( text_anchor & GfxSystem::ANCHOR_VCENTER ) {
			offset.y = -offset.y/2;
		} else if ( text_anchor & GfxSystem::ANCHOR_BOTTOM ) {
			offset.y = -offset.y;
		} else if ( text_anchor & GfxSystem::ANCHOR_TOP ) {
			offset.y = 0;
		}
		if ( text_anchor & GfxSystem::ANCHOR_HCENTER ) {
			offset.x = -offset.x/2;
		} else if ( text_anchor & GfxSystem::ANCHOR_RIGHT ) {
			offset.x = -offset.x;
		} else if ( text_anchor & GfxSystem::ANCHOR_LEFT ) {
			offset.x = 0;
		}
			
		if ( screen_anchor & GfxSystem::ANCHOR_VCENTER ) {			
			mElement->setYPosition(CEGUI::UDim(0.5f + y + offset.y, 0));
		} else if ( screen_anchor & GfxSystem::ANCHOR_BOTTOM ) {
			mElement->setYPosition(CEGUI::UDim(1.0f - y + offset.y, 0));
		} else if ( screen_anchor & GfxSystem::ANCHOR_TOP ) {
			mElement->setYPosition(CEGUI::UDim(y + offset.y, 0));
		}
		if ( screen_anchor & GfxSystem::ANCHOR_HCENTER ) {			
			mElement->setXPosition(CEGUI::UDim(0.5f + x + offset.x, 0));
		} else if ( screen_anchor & GfxSystem::ANCHOR_RIGHT ) {
			mElement->setXPosition(CEGUI::UDim(1.0f - x + offset.x, 0));
		} else if ( screen_anchor & GfxSystem::ANCHOR_LEFT ) {
			mElement->setXPosition(CEGUI::UDim(x + offset.x, 0));
		}	
	}

	void StaticElement::SetPosition( int32 x, int32 y,
			uint8 text_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			uint8 screen_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/ )
	{
		SetPosition( (float)x/(float)gGfxRenderer.GetScreenWidth(),
			(float)y/(float)gGfxRenderer.GetScreenHeight(),
			text_anchor, screen_anchor);
	}

	void StaticElement::SetSize() {
		mElement->setWidth(
			CEGUI::UDim( 0, mElement->getFont()->getTextExtent( mElement->getText() )) );
		mElement->setHeight(
			CEGUI::UDim( 0, mElement->getFont()->getFontHeight()) );
	}

	Vector2 StaticElement::GetSize() {
		CEGUI::UVector2 size = mElement->getSize();
		
		return Vector2(size.d_x.asRelative((float)gGfxRenderer.GetScreenWidth()),
			size.d_y.asRelative((float)gGfxRenderer.GetScreenHeight()));
	}

	StaticText::StaticText( float32 x, float32 y, const string & id, const string & text, 
			const GfxSystem::Color & color,
			uint8 text_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			uint8 screen_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			const string & fontid/* = ""*/ )
	{
		InitElement(id);
		SetStaticText( x, y, text, color, text_anchor, screen_anchor, fontid );
	}

	StaticText::StaticText( int32 x, int32 y, const string & id, const string & text,
			const GfxSystem::Color & color,
			uint8 text_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			uint8 screen_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			const string & fontid/* = ""*/ )
	{
		InitElement(id);
		SetStaticText( x, y, text, color, text_anchor, screen_anchor, fontid );
	}

	void StaticText::SetStaticText( float32 x, float32 y, const string & text,
			const GfxSystem::Color & color,
			uint8 text_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			uint8 screen_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			const string & fontid/* = ""*/ )
	{
		SetStaticTextWorker( text, color, fontid );
		SetPosition( x, y, text_anchor, screen_anchor );
	}

	void StaticText::SetStaticText( int32 x, int32 y, const string & text,
			const GfxSystem::Color & color,
			uint8 text_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			uint8 screen_anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			const string & fontid/* = ""*/ )
	{
		SetStaticTextWorker( text, color, fontid );
		SetPosition( x, y, text_anchor, screen_anchor );
	}

	void StaticText::SetStaticTextWorker( const string & text,
		const GfxSystem::Color & color, const string & fontid/* = "" */)
	{
		SetText( text );
		SetFont( fontid );
		SetColor( color );
	}

	StaticElement::~StaticElement() {
		CEGUI::System::getSingleton().getGUISheet()->removeChildWindow( mElement );
		CEGUI::WindowManager::getSingleton().destroyWindow( mElement );
	}

	StaticText::~StaticText() {
	}

	void StaticText::InitElement( const string & id ) {
		mElement = CEGUI::WindowManager::getSingleton().createWindow( "Lightweight/StaticText", "CustomStaticText/" + id );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow( mElement );
		mElement->setProperty("VertFormatting", "TopAligned");
	}

	void StaticText::SetColor( const GfxSystem::Color & color ) {
		//TODO std pryc!
		std::stringstream sstream;
		uint32 coloruint = color.GetARGB();
		sstream << std::hex << std::uppercase << std::setfill('0') <<
			"tl:" << std::setw(8) << coloruint << " tr:" << std::setw(8) << coloruint <<
			" bl:" << std::setw(8) << coloruint << " br:" << std::setw(8) << coloruint;

		mElement->setProperty( "TextColours", sstream.str() );
	}

	void StaticText::SetFont( const string & fontid ) {
		if (fontid == "") return;
		try {			
			if( !CEGUI::FontManager::getSingleton().isFontPresent( fontid ) )
				CEGUI::FontManager::getSingleton().createFont( fontid + ".font" );
			mElement->setFont( CEGUI::FontManager::getSingleton().getFont( fontid ) );			
		} catch (...) {
			gLogMgr.LogMessage( "ERROR!!! Font " + fontid + " not found." );
		}
	}

	void StaticText::SetText( const string & text ) {
		mElement->setText( text );
	}
}