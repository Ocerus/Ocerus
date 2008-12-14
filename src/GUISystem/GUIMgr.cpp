#include "Common.h"
#include "GUIMgr.h"
#include "CEGUI.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/InputActions.h"
#include "RendererGate.h"
#include "ResourceGate.h"

namespace GUISystem {
	/// Prototypes for utility functions
	//@{
	CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key);
	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn);
	//@}

	GUIMgr::GUIMgr() : ConsoleIsLoaded(false) {
		gLogMgr.LogMessage("******** GUIMgr init *********");
		mCegui = DYN_NEW CEGUI::System( mRendererGate = DYN_NEW RendererGate(), mResourceGate = DYN_NEW ResourceGate() );
		gInputMgr.AddInputListener(this);

		CEGUI::Imageset::setDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");		
	}

	void GUIMgr::LoadGUI() {
		gLogMgr.LogMessage("******** GUI Menu init *********");
		/*
		gResourceMgr.AddResourceDirToGroup("gui/schemes", "schemes");
		gResourceMgr.AddResourceDirToGroup("gui/imagesets", "imagesets");
		gResourceMgr.AddResourceDirToGroup("gui/fonts", "fonts");
		gResourceMgr.AddResourceDirToGroup("gui/layouts", "layouts");
		gResourceMgr.AddResourceDirToGroup("gui/looknfeel", "looknfeels");
		*/
		//CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme");
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
		
	}

	void GUIMgr::LoadConsole() {
		gLogMgr.LogMessage("******** GUI Console init *********");
// Resource loading
		gResourceMgr.AddResourceFileToGroup("gui/schemes/Console.scheme", "schemes");
		gResourceMgr.AddResourceFileToGroup("gui/schemes/TaharezLook.scheme", "schemes");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/Console.imageset", "imagesets");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/BSLogov2.png", "imagesets");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/TaharezLook.imageset", "imagesets");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/TaharezLook.tga", "imagesets");		
		gResourceMgr.AddResourceFileToGroup("gui/fonts/Commonwealth-10.font", "fonts");
		gResourceMgr.AddResourceFileToGroup("gui/fonts/Commonv2c.ttf", "fonts");
		gResourceMgr.AddResourceFileToGroup("gui/layouts/Console.layout", "layouts");
		gResourceMgr.AddResourceFileToGroup("gui/looknfeel/TaharezLook.looknfeel", "looknfeels");
/* // Resource blind loading

		gResourceMgr.AddResourceDirToGroup("gui/schemes", "schemes");
		gResourceMgr.AddResourceDirToGroup("gui/imagesets", "imagesets");
		gResourceMgr.AddResourceDirToGroup("gui/fonts", "fonts");
		gResourceMgr.AddResourceDirToGroup("gui/layouts", "layouts");
		gResourceMgr.AddResourceDirToGroup("gui/looknfeel", "looknfeels");
*/
		CEGUI::SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
		CEGUI::SchemeManager::getSingleton().loadScheme("Console.scheme");

		if( !CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
			CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );

		mCegui->setDefaultFont( "Commonwealth-10" );

		CurrentWindowRoot =
			CEGUI::WindowManager::getSingleton().loadWindowLayout( "Console.layout" );

		CEGUI::System::getSingleton().setGUISheet( CurrentWindowRoot );
		
		RegisterEvents();

	}

	void GUIMgr::AddConsoleListener(IConsoleListener* listener) {
		ConsoleListeners.insert(listener);
	}

	void GUIMgr::RegisterEvents() {
		assert(mCegui);
		/*
		CEGUI::Window* quit_button = CEGUI::WindowManager::getSingleton().getWindow("Root/QuitButton");
		if (quit_button)
			quit_button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUIMgr::QuitEvent, this));
		*/

		CEGUI::Window* console = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		console->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&GUIMgr::ConsoleCommandEvent, this));	
	}

	void GUIMgr::ConsoleTrigger() {
		EnsureConsoleIsLoaded();
		CEGUI::Window* Root = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot");
		Root->setVisible( !Root->isVisible() );

		CEGUI::Window* Console = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		Console->activate();			
	}

	bool GUIMgr::QuitEvent(const CEGUI::EventArgs& e)
	{
		gApp.RequestStateChange(Core::AS_SHUTDOWN);
		return true;
	}

	bool GUIMgr::ConsoleCommandEvent(const CEGUI::EventArgs& e) {
		CEGUI::Window* prompt = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		AddConsoleMessage(prompt->getText().c_str());

		prompt->setText("");

		return true;
	}

	void GUIMgr::AddConsoleMessage(std::string message, const GfxSystem::Color& color) {
		EnsureConsoleIsLoaded();
		CEGUI::Listbox* pane = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/Pane");

		CEGUI::ListboxTextItem* new_item = DYN_NEW CEGUI::ListboxTextItem(message);
		new_item->setTextColours(CEGUI::colour(color.r, color.g, color.b, color.a));

		pane->addItem(new_item);
		pane->ensureItemIsVisible(new_item);
		
		uint32 item_count;
		while (item_count = pane->getItemCount() > 50)
			pane->removeItem(pane->getListboxItemFromIndex(item_count - 1));

		std::set<IConsoleListener*>::iterator iter = ConsoleListeners.begin();
		while (iter != ConsoleListeners.end())
		{
			(*iter)->EventConsoleCommand(message);
			++iter;
		}
	}

	void GUIMgr::Update( float32 delta ) {
		assert(mCegui);
		mCegui->injectTimePulse( delta );
	}

	void GUIMgr::KeyPressed(const InputSystem::KeyInfo& ke) {
		assert(mCegui);

		if (ke.keyAction == InputSystem::KC_GRAVE) {
			ConsoleTrigger();
			return;
		}

		bool res = mCegui->injectKeyDown(KeyMapperOIStoCEGUI(ke.keyAction));
		if (!res)			
			mCegui->injectChar(ke.keyCode);
	}

	void GUIMgr::KeyReleased(const InputSystem::KeyInfo& ke) {
		assert(mCegui);
		CEGUI::System::getSingleton().injectKeyUp(ke.keyCode);
	}

	void GUIMgr::MouseMoved(const InputSystem::MouseInfo& mi) {
		assert(mCegui);
		CEGUI::System::getSingleton().injectMousePosition(float(mi.x), float(mi.y));
		CEGUI::System::getSingleton().injectMouseWheelChange(float(mi.wheelDelta));
	}

	void GUIMgr::MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn) {
		assert(mCegui);
		CEGUI::System::getSingleton().injectMouseButtonDown( ConvertMouseButtonEnum(btn) );
	}

	void GUIMgr::MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn) {
		assert(mCegui);
		CEGUI::System::getSingleton().injectMouseButtonUp( ConvertMouseButtonEnum(btn) );
	}

	void GUIMgr::EnsureConsoleIsLoaded() {
		if (!ConsoleIsLoaded) {
			LoadConsole();
			ConsoleIsLoaded = true;
		}
	}

	GUIMgr::~GUIMgr() {
		gInputMgr.RemoveInputListener(this);
/*
		if (mCegui)
			DYN_DELETE mCegui;
			*/
		if (mRendererGate)
			DYN_DELETE mRendererGate;
		if (mResourceGate)
			DYN_DELETE mResourceGate;
	}

	CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key)
	{
		switch (key) {
			case InputSystem::KC_RETURN:
				return CEGUI::Key::Return;
			case InputSystem::KC_UP:
				return CEGUI::Key::ArrowUp;
			case InputSystem::KC_DOWN:
				return CEGUI::Key::ArrowDown;
			case InputSystem::KC_RIGHT:
				return CEGUI::Key::ArrowRight;
			case InputSystem::KC_LEFT:
				return CEGUI::Key::ArrowLeft;
			case InputSystem::KC_BACK:
				return CEGUI::Key::Backspace;
			default:
				return key;
		}
	}

	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn) {
		switch (btn) {
			case InputSystem::MBTN_LEFT:
				return CEGUI::LeftButton;
			case InputSystem::MBTN_RIGHT:
				return CEGUI::RightButton;
			case InputSystem::MBTN_MIDDLE:
				return CEGUI::MiddleButton;
		}
		return CEGUI::NoButton;
	}
}
