#include "Common.h"
#include "StaticElements.h"
#include "CEGUIBase.h"
#include "CEGUIWindowManager.h"
#include "CEGUIFontManager.h"
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

	void StaticElement::SetPosition( float32 x, float32 y, uint8 anchor ) {
		mElement->setArea( CEGUI::UDim( 0.0f,0 ), CEGUI::UDim(0.0f,0), CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0));		
		if ( anchor & GfxSystem::ANCHOR_VCENTER ) {			
			mElement->setYPosition(CEGUI::UDim(0.5f + y, 0));
		} else if ( anchor & GfxSystem::ANCHOR_TOP ) {
			mElement->setYPosition(CEGUI::UDim(1.0f - y, 0));
		} else if ( anchor & GfxSystem::ANCHOR_BOTTOM ) {
			mElement->setYPosition(CEGUI::UDim(y, 0));
		}

		if ( anchor & GfxSystem::ANCHOR_HCENTER ) {			
			mElement->setXPosition(CEGUI::UDim(0.5f + x, 0));
		} else if ( anchor & GfxSystem::ANCHOR_RIGHT ) {
			mElement->setXPosition(CEGUI::UDim(1.0f - x, 0));
		} else if ( anchor & GfxSystem::ANCHOR_LEFT ) {
			mElement->setXPosition(CEGUI::UDim(x, 0));
		}	
	}

	void StaticElement::SetPosition( int32 x, int32 y, uint8 anchor ) {
		if ( anchor & GfxSystem::ANCHOR_HCENTER ) {			
			mElement->setXPosition(CEGUI::UDim(0.5f, (float32)x));
		} else if ( anchor & GfxSystem::ANCHOR_RIGHT ) {
			mElement->setXPosition(CEGUI::UDim(1.0f, (float32)-x));
		} else if ( anchor & GfxSystem::ANCHOR_LEFT ) {
			mElement->setXPosition(CEGUI::UDim(0, (float32)x));
		}

		if ( anchor & GfxSystem::ANCHOR_VCENTER ) {			
			mElement->setYPosition(CEGUI::UDim(0.5f, (float32)y));
		} else if ( anchor & GfxSystem::ANCHOR_TOP ) {
			mElement->setYPosition(CEGUI::UDim(1.0f, (float32)-y));
		} else if ( anchor & GfxSystem::ANCHOR_BOTTOM ) {
			mElement->setYPosition(CEGUI::UDim(0, (float32)y));
		}
	}

	void StaticElement::SetSize() {
		mElement->setWidth( CEGUI::UDim( mElement->getFont()->getTextExtent( mElement->getText() ), 0) );
		mElement->setHeight( CEGUI::UDim( mElement->getFont()->getFontHeight(), 0) );
	}

	Vector2 StaticElement::GetSize() {
		CEGUI::UVector2 size = mElement->getSize();
		return Vector2( size.d_x.d_scale, size.d_y.d_scale );
	}

	StaticText::StaticText( float32 x, float32 y, const std::string & text, 
			const GfxSystem::Color & color,
			uint8 anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			const std::string & fontid/* = ""*/ )
	{
		InitElement();
		SetPosition( x, y, anchor );		
		SetColor( color );
		SetFont( fontid );
		SetText( text );
		SetSize();		
	}

	StaticText::StaticText( int32 x, int32 y, const std::string & text,
			const GfxSystem::Color & color,
			uint8 anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/,
			const std::string & fontid/* = ""*/ )
	{
		InitElement();
		SetPosition( x, y, anchor );
		SetColor( color );
		SetFont( fontid );		
		SetText( text );
		SetSize();
	}

	StaticElement::~StaticElement() {
		CEGUI::System::getSingleton().getGUISheet()->removeChildWindow( mElement );
		CEGUI::WindowManager::getSingleton().destroyWindow( mElement );
	}

	StaticText::~StaticText() {
	}

	void StaticText::InitElement() {
		std::stringstream sstream;
		sstream << "StaticTextOnDemand" << mID++;
		mElement = CEGUI::WindowManager::getSingleton().createWindow( "Lightweight/StaticText", sstream.str() );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow( mElement );
		mElement->setProperty("VertFormatting", "TopAligned");
	}

	void StaticText::SetColor( const GfxSystem::Color & color ) {
		std::stringstream sstream;
		uint32 coloruint = color.GetARGB();
		sstream << std::hex << std::uppercase << std::setfill('0') <<
			"tl:" << std::setw(8) << coloruint << " tr:" << std::setw(8) << coloruint <<
			" bl:" << std::setw(8) << coloruint << " br:" << std::setw(8) << coloruint;

		mElement->setProperty( "TextColours", sstream.str() );
	}

	void StaticText::SetFont( const std::string & fontid ) {
		if (fontid == "") return;
		try {			
			if( !CEGUI::FontManager::getSingleton().isFontPresent( fontid ) )
				CEGUI::FontManager::getSingleton().createFont( fontid + ".font" );
			mElement->setFont( CEGUI::FontManager::getSingleton().getFont( fontid ) );			
		} catch (...) {
			gLogMgr.LogMessage( "ERROR!!! Font " + fontid + " not found." );
		}
	}

	void StaticText::SetText( const std::string & text ) {
		mElement->setText( text );
	}
}