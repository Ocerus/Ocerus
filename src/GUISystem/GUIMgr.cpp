#include "Common.h"
#include "GUIMgr.h"
#include "CEGUI.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/InputActions.h"
#include "RendererGate.h"
#include "ResourceGate.h"

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
		CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme");
		CEGUI::SchemeManager::getSingleton().loadScheme( "BGGUI.scheme");

		// load in a font.  The first font loaded automatically becomes the default font.
		if( !CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
		  CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );

		mCegui->setDefaultFont( "Commonwealth-10" );
		//CEGUI::System::getSingleton().setDefaultMouseCursor( "Vanilla", "MouseArrow" );
		
		std::string layout = gApp.GetGlobalConfig()->GetString("Layout", "Battleships.layout", "CEGUI");

		CEGUI::Window* CurrentWindowRoot =
			CEGUI::WindowManager::getSingleton().loadWindowLayout( layout.c_str() );

		
		CEGUI::ImagesetManager::ImagesetIterator iter = CEGUI::ImagesetManager::getSingleton().getIterator();

		CEGUI::System::getSingleton().setGUISheet( CurrentWindowRoot );

		//CurrentWindowRoot->setProperty("Image", CEGUI::PropertyHelper::imageToString(space));

		gInputMgr.AddInputListener(this);		
	}

	GUIMgr::~GUIMgr() {
		gInputMgr.RemoveInputListener(this);
		DYN_DELETE mCegui;
		DYN_DELETE mRendererGate;
		DYN_DELETE mResourceGate;
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
		mCegui->injectTimePulse( delta );
	}

}
