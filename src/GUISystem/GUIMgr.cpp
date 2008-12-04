#include "Common.h"
#include "GUIMgr.h"
#include "CEGUI.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/InputActions.h"
#include "RendererGate.h"
#include "ResourceGate.h"

namespace GUISystem {
	GUIMgr::GUIMgr() : mCegui(0), mRendererGate(0), mResourceGate(0) {
	}

	void GUIMgr::LoadGUI() {
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
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		
		std::string layout = gApp.GetGlobalConfig()->GetString("Layout", "Battleships.layout", "CEGUI");

		CEGUI::Window* CurrentWindowRoot =
			CEGUI::WindowManager::getSingleton().loadWindowLayout( layout.c_str() );
		
		CEGUI::System::getSingleton().setGUISheet( CurrentWindowRoot );

		RegisterEvents();

		gInputMgr.AddInputListener(this);
	}

	GUIMgr::~GUIMgr() {
		gInputMgr.RemoveInputListener(this);
		if (mCegui)
			DYN_DELETE mCegui;
		if (mRendererGate)
			DYN_DELETE mRendererGate;
		if (mResourceGate)
			DYN_DELETE mResourceGate;
	}

	void GUIMgr::RegisterEvents() {
		assert(mCegui && CurrentWindowRoot);
		CEGUI::Window* quit_button = CEGUI::WindowManager::getSingleton().getWindow("Root/QuitButton");
		if (quit_button)
			quit_button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUIMgr::QuitEvent, this));
	}

	bool GUIMgr::QuitEvent(const CEGUI::EventArgs& e)
	{
		gApp.RequestStateChange(Core::AS_SHUTDOWN);
		return true;
	}

	void GUIMgr::Update( float32 delta ) {
		assert(mCegui);
		mCegui->injectTimePulse( delta );
	}

}
