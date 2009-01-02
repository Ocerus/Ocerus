#include "Common.h"
#include "GUIMgr.h"
#include "CEGUI.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/InputActions.h"
#include "RendererGate.h"
#include "ResourceGate.h"
#include <sstream>
#include <math.h>

namespace GUISystem {
	/// @name Prototypes for utility functions
	//@{
	CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key);
	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn);
	//@}

	GUIMgr::GUIMgr() : mConsoleIsLoaded(false) {
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

	void GUIMgr::LoadStyle( void )
	{
		gLogMgr.LogMessage( "load style" );
		gResourceMgr.AddResourceDirToGroup("gui/schemes", "schemes");
		gResourceMgr.AddResourceDirToGroup("gui/imagesets", "imagesets");
		gResourceMgr.AddResourceDirToGroup("gui/fonts", "fonts");
		gResourceMgr.AddResourceDirToGroup("gui/layouts", "layouts");
		gResourceMgr.AddResourceDirToGroup("gui/looknfeel", "looknfeels");

		CEGUI::SchemeManager::getSingleton().loadScheme("Lightweight.scheme");
		if( !CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
			CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );

		CEGUI::SchemeManager::getSingleton().loadScheme("Console.scheme");
		CurrentWindowRoot =	CEGUI::WindowManager::getSingleton().loadWindowLayout( "Console.layout" );
		CEGUI::System::getSingleton().setGUISheet( CurrentWindowRoot );
		mConsoleIsLoaded = true;

		mCegui->setDefaultFont( "Commonwealth-10" );
		mCegui->setDefaultMouseCursor( "Lightweight", "MouseArrow" );
		RegisterEvents();
	}


	void GUIMgr::AddConsoleListener(IConsoleListener* listener) {
		ConsoleListeners.insert(listener);
	}

	void GUIMgr::RegisterEvents() {
		assert(mCegui);

		CEGUI::Window* console = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		console->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&GUIMgr::ConsoleCommandEvent, this));	
	}

	void GUIMgr::ConsoleTrigger() {
		CEGUI::Window* Root = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot");
		Root->setVisible( !Root->isVisible() );

		CEGUI::Window* Console = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		Console->activate();
	}

	bool GUIMgr::QuitEvent(const CEGUI::EventArgs& e) {
		gApp.RequestStateChange(Core::AS_SHUTDOWN);
		return true;
	}

	bool GUIMgr::ConsoleCommandEvent(const CEGUI::EventArgs& e) {
		CEGUI::Window* prompt = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		std::string message(prompt->getText().c_str());
		if (message == "quit")
			gApp.RequestStateChange( Core::AS_SHUTDOWN );
		if ( message.length() >= 9 ) {
			std::string prefix = message.substr(0, 7);
			std::string args = message.substr(8);
			if (prefix == "addtext")
				AddStaticText( GetTextSize(args).x, 0.25f, args, GfxSystem::Color( 255, 0, 0),
					GfxSystem::ANCHOR_VCENTER | GfxSystem::ANCHOR_RIGHT );
		}
		AddConsoleMessage(message);
		prompt->setText("");

		return true;
	}

	Vector2 GUIMgr::GetTextSize( const std::string & text, const std::string & fontid ) {
		CEGUI::Font* font;
		if (fontid != "")
			font = CEGUI::FontManager::getSingleton().getFont(fontid);
		else
			font = CEGUI::System::getSingleton().getDefaultFont();
		return Vector2(font->getTextExtent(text)/gGfxRenderer.GetScreenWidth(),
			font->getFontHeight()/gGfxRenderer.GetScreenHeight());
	}

	void GUIMgr::AddConsoleMessage(std::string message, const GfxSystem::Color& color) {
		if (!mConsoleIsLoaded)
			return;

		CEGUI::Listbox* pane = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/Pane");

		CEGUI::ListboxTextItem* new_item = DYN_NEW CEGUI::ListboxTextItem(message);
		new_item->setTextColours(CEGUI::colour(color.a, color.r, color.g, color.b));

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
		InputSystem::MouseState& m = gInputMgr.GetMouseState();
		mCegui->injectMousePosition(float(m.x), float(m.y));
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
		CEGUI::System::getSingleton().injectKeyUp(KeyMapperOIStoCEGUI(ke.keyAction));
	}

	void GUIMgr::MouseMoved(const InputSystem::MouseInfo& mi) {
		assert(mCegui);
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

	StaticText* GUIMgr::AddStaticText( float32 x, float32 y, const std::string & text,
			const GfxSystem::Color color/* = GfxSystem::Color(255,255,255)*/,
			uint8 anchor/* = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP*/ ) {
		StaticText* ptr = new StaticText( x, y, text, color, anchor );
		CreatedStaticElements.push_back( ptr );
		return ptr;
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
			case InputSystem::KC_NUMPADENTER:
				return CEGUI::Key::NumpadEnter;
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
		return CEGUI::LeftButton;
	}
}
