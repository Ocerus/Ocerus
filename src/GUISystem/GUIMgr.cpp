#include "../LogSystem/LogMgr.h"
#include "GUIMgr.h"
#include "CEGUI.h"

namespace GUISystem {
	GUIMgr::GUIMgr() {
		gLogMgr.LogMessage("******** GUIMgr init *********");
		mCegui = DYN_NEW CEGUI::System( mRendererGate = DYN_NEW RendererGate(), mResourceGate = DYN_NEW ResourceGate() );

		gResourceMgr.AddResourceDirToGroup("gui/schemes", "schemes");
		gResourceMgr.AddResourceDirToGroup("gui/imagesets", "imagesets");
		gResourceMgr.AddResourceDirToGroup("gui/fonts", "fonts");
		gResourceMgr.AddResourceDirToGroup("gui/layouts", "layouts");
		gResourceMgr.AddResourceDirToGroup("gui/looknfeel", "looknfeels");

		CEGUI::Imageset::setDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");

		// load in the scheme file, which auto-loads the TaharezLook imageset
		CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme" );

		// load in a font.  The first font loaded automatically becomes the default font.
		if(! CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
		  CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );

		CEGUI::System::getSingleton().setDefaultFont( "Commonwealth-10" );
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::System::getSingleton().setDefaultTooltip( "TaharezLook/Tooltip" );

		CEGUI::Window* myRoot = CEGUI::WindowManager::getSingleton().loadWindowLayout( "EditorDefault.layout" );
		CEGUI::System::getSingleton().setGUISheet( myRoot );

		gInputMgr.AddInputListener(this);		
	}

	GUIMgr::~GUIMgr() {
		gInputMgr.RemoveInputListener(this);
		DYN_DELETE mCegui;
	}

/*
// bool injectMouseMove( float delta_x, float delta_y );
+ bool injectMousePosition( float x_pos, float y_pos );
// bool injectMouseLeaves( void );
+ bool injectMouseButtonDown( MouseButton button );
+ bool injectMouseButtonUp( MouseButton button );
+ bool injectKeyDown( uint key_code );
+ bool injectKeyUp( uint key_code );
// bool injectChar( utf32 code_point );
+ bool injectMouseWheelChange( float delta );
- bool injectTimePulse( float timeElapsed ); - bude v jine casti
*/

	void GUIMgr::Update( float32 delta ) {
		
	}

}
