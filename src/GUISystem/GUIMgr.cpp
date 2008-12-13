#include "Common.h"
#include "GUIMgr.h"
#include "CEGUI.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/InputActions.h"
#include "RendererGate.h"
#include "ResourceGate.h"

namespace GUISystem {
	CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key);
	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn);

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
		// load in the scheme file, which auto-loads the TaharezLook imageset
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
/*

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
/*
	enum eKeyCode
	{
		KC_UNASSIGNED  = 0x00,
		KC_ESCAPE      = 0x01,
		KC_1           = 0x02,
		KC_2           = 0x03,
		KC_3           = 0x04,
		KC_4           = 0x05,
		KC_5           = 0x06,
		KC_6           = 0x07,
		KC_7           = 0x08,
		KC_8           = 0x09,
		KC_9           = 0x0A,
		KC_0           = 0x0B,
		KC_MINUS       = 0x0C,    // - on main keyboard
		KC_EQUALS      = 0x0D,
		KC_BACK        = 0x0E,    // backspace
		KC_TAB         = 0x0F,
		KC_Q           = 0x10,
		KC_W           = 0x11,
		KC_E           = 0x12,
		KC_R           = 0x13,
		KC_T           = 0x14,
		KC_Y           = 0x15,
		KC_U           = 0x16,
		KC_I           = 0x17,
		KC_O           = 0x18,
		KC_P           = 0x19,
		KC_LBRACKET    = 0x1A,
		KC_RBRACKET    = 0x1B,
		KC_RETURN      = 0x1C,    // Enter on main keyboard
		KC_LCONTROL    = 0x1D,
		KC_A           = 0x1E,
		KC_S           = 0x1F,
		KC_D           = 0x20,
		KC_F           = 0x21,
		KC_G           = 0x22,
		KC_H           = 0x23,
		KC_J           = 0x24,
		KC_K           = 0x25,
		KC_L           = 0x26,
		KC_SEMICOLON   = 0x27,
		KC_APOSTROPHE  = 0x28,
		KC_GRAVE       = 0x29,    // accent
		KC_LSHIFT      = 0x2A,
		KC_BACKSLASH   = 0x2B,
		KC_Z           = 0x2C,
		KC_X           = 0x2D,
		KC_C           = 0x2E,
		KC_V           = 0x2F,
		KC_B           = 0x30,
		KC_N           = 0x31,
		KC_M           = 0x32,
		KC_COMMA       = 0x33,
		KC_PERIOD      = 0x34,    // . on main keyboard
		KC_SLASH       = 0x35,    // / on main keyboard
		KC_RSHIFT      = 0x36,
		KC_MULTIPLY    = 0x37,    // * on numeric keypad
		KC_LMENU       = 0x38,    // left Alt
		KC_SPACE       = 0x39,
		KC_CAPITAL     = 0x3A,
		KC_F1          = 0x3B,
		KC_F2          = 0x3C,
		KC_F3          = 0x3D,
		KC_F4          = 0x3E,
		KC_F5          = 0x3F,
		KC_F6          = 0x40,
		KC_F7          = 0x41,
		KC_F8          = 0x42,
		KC_F9          = 0x43,
		KC_F10         = 0x44,
		KC_NUMLOCK     = 0x45,
		KC_SCROLL      = 0x46,    // Scroll Lock
		KC_NUMPAD7     = 0x47,
		KC_NUMPAD8     = 0x48,
		KC_NUMPAD9     = 0x49,
		KC_SUBTRACT    = 0x4A,    // - on numeric keypad
		KC_NUMPAD4     = 0x4B,
		KC_NUMPAD5     = 0x4C,
		KC_NUMPAD6     = 0x4D,
		KC_ADD         = 0x4E,    // + on numeric keypad
		KC_NUMPAD1     = 0x4F,
		KC_NUMPAD2     = 0x50,
		KC_NUMPAD3     = 0x51,
		KC_NUMPAD0     = 0x52,
		KC_DECIMAL     = 0x53,    // . on numeric keypad
		KC_OEM_102     = 0x56,    // < > | on UK/Germany keyboards
		KC_F11         = 0x57,
		KC_F12         = 0x58,
		KC_F13         = 0x64,    //                     (NEC PC98)
		KC_F14         = 0x65,    //                     (NEC PC98)
		KC_F15         = 0x66,    //                     (NEC PC98)
		KC_KANA        = 0x70,    // (Japanese keyboard)
		KC_ABNT_C1     = 0x73,    // / ? on Portugese (Brazilian) keyboards
		KC_CONVERT     = 0x79,    // (Japanese keyboard)
		KC_NOCONVERT   = 0x7B,    // (Japanese keyboard)
		KC_YEN         = 0x7D,    // (Japanese keyboard)
		KC_ABNT_C2     = 0x7E,    // Numpad . on Portugese (Brazilian) keyboards
		KC_NUMPADEQUALS= 0x8D,    // = on numeric keypad (NEC PC98)
		KC_PREVTRACK   = 0x90,    // Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
		KC_AT          = 0x91,    //                     (NEC PC98)
		KC_COLON       = 0x92,    //                     (NEC PC98)
		KC_UNDERLINE   = 0x93,    //                     (NEC PC98)
		KC_KANJI       = 0x94,    // (Japanese keyboard)
		KC_STOP        = 0x95,    //                     (NEC PC98)
		KC_AX          = 0x96,    //                     (Japan AX)
		KC_UNLABELED   = 0x97,    //                        (J3100)
		KC_NEXTTRACK   = 0x99,    // Next Track
		KC_NUMPADENTER = 0x9C,    // Enter on numeric keypad
		KC_RCONTROL    = 0x9D,
		KC_MUTE        = 0xA0,    // Mute
		KC_CALCULATOR  = 0xA1,    // Calculator
		KC_PLAYPAUSE   = 0xA2,    // Play / Pause
		KC_MEDIASTOP   = 0xA4,    // Media Stop
		KC_VOLUMEDOWN  = 0xAE,    // Volume -
		KC_VOLUMEUP    = 0xB0,    // Volume +
		KC_WEBHOME     = 0xB2,    // Web home
		KC_NUMPADCOMMA = 0xB3,    // , on numeric keypad (NEC PC98)
		KC_DIVIDE      = 0xB5,    // / on numeric keypad
		KC_SYSRQ       = 0xB7,
		KC_RMENU       = 0xB8,    // right Alt
		KC_PAUSE       = 0xC5,    // Pause
		KC_HOME        = 0xC7,    // Home on arrow keypad
		KC_UP          = 0xC8,    // UpArrow on arrow keypad
		KC_PGUP        = 0xC9,    // PgUp on arrow keypad
		KC_LEFT        = 0xCB,    // LeftArrow on arrow keypad
		KC_RIGHT       = 0xCD,    // RightArrow on arrow keypad
		KC_END         = 0xCF,    // End on arrow keypad
		KC_DOWN        = 0xD0,    // DownArrow on arrow keypad
		KC_PGDOWN      = 0xD1,    // PgDn on arrow keypad
		KC_INSERT      = 0xD2,    // Insert on arrow keypad
		KC_DELETE      = 0xD3,    // Delete on arrow keypad
		KC_LWIN        = 0xDB,    // Left Windows key
		KC_RWIN        = 0xDC,    // Right Windows key
		KC_APPS        = 0xDD,    // AppMenu key
		KC_POWER       = 0xDE,    // System Power
		KC_SLEEP       = 0xDF,    // System Sleep
		KC_WAKE        = 0xE3,    // System Wake
		KC_WEBSEARCH   = 0xE5,    // Web Search
		KC_WEBFAVORITES= 0xE6,    // Web Favorites
		KC_WEBREFRESH  = 0xE7,    // Web Refresh
		KC_WEBSTOP     = 0xE8,    // Web Stop
		KC_WEBFORWARD  = 0xE9,    // Web Forward
		KC_WEBBACK     = 0xEA,    // Web Back
		KC_MYCOMPUTER  = 0xEB,    // My Computer
		KC_MAIL        = 0xEC,    // Mail
		KC_MEDIASELECT = 0xED     // Media Select
	};
	enum Scan
    {
        Escape          =0x01,
        One             =0x02,
        Two             =0x03,
        Three           =0x04,
        Four            =0x05,
        Five            =0x06,
        Six             =0x07,
        Seven           =0x08,
        Eight           =0x09,
        Nine            =0x0A,
        Zero            =0x0B,
        Minus           =0x0C,    // - on main keyboard
        Equals			=0x0D,
        Backspace		=0x0E,    // backspace
        Tab				=0x0F,
        Q               =0x10,
        W               =0x11,
        E               =0x12,
        R               =0x13,
        T               =0x14,
        Y               =0x15,
        U               =0x16,
        I               =0x17,
        O               =0x18,
        P               =0x19,
        LeftBracket     =0x1A,
        RightBracket    =0x1B,
        Return			=0x1C,    // Enter on main keyboard
        LeftControl		=0x1D,
        A               =0x1E,
        S               =0x1F,
        D               =0x20,
        F               =0x21,
        G               =0x22,
        H               =0x23,
        J               =0x24,
        K               =0x25,
        L               =0x26,
        Semicolon       =0x27,
        Apostrophe		=0x28,
        Grave           =0x29,    // accent grave
        LeftShift       =0x2A,
        Backslash       =0x2B,
        Z               =0x2C,
        X               =0x2D,
        C               =0x2E,
        V               =0x2F,
        B               =0x30,
        N               =0x31,
        M               =0x32,
        Comma           =0x33,
        Period          =0x34,    // . on main keyboard
        Slash           =0x35,    // '/' on main keyboard
        RightShift      =0x36,
        Multiply        =0x37,    // * on numeric keypad
        LeftAlt        =0x38,     // left Alt
        Space           =0x39,
        Capital         =0x3A,
        F1              =0x3B,
        F2              =0x3C,
        F3              =0x3D,
        F4              =0x3E,
        F5              =0x3F,
        F6              =0x40,
        F7              =0x41,
        F8              =0x42,
        F9              =0x43,
        F10             =0x44,
        NumLock         =0x45,
        ScrollLock      =0x46,    // Scroll Lock
        Numpad7         =0x47,
        Numpad8         =0x48,
        Numpad9         =0x49,
        Subtract        =0x4A,    // - on numeric keypad
        Numpad4         =0x4B,
        Numpad5         =0x4C,
        Numpad6         =0x4D,
        Add				=0x4E,    // + on numeric keypad
        Numpad1         =0x4F,
        Numpad2         =0x50,
        Numpad3         =0x51,
        Numpad0         =0x52,
        Decimal			=0x53,    // . on numeric keypad
        OEM_102         =0x56,    // < > | on UK/Germany keyboards
        F11             =0x57,
        F12             =0x58,
        F13             =0x64,    //                     (NEC PC98) 
        F14             =0x65,    //                     (NEC PC98) 
        F15             =0x66,    //                     (NEC PC98) 
        Kana            =0x70,    // (Japanese keyboard)            
        ABNT_C1         =0x73,    // / ? on Portugese (Brazilian) keyboards 
        Convert         =0x79,    // (Japanese keyboard)            
        NoConvert       =0x7B,    // (Japanese keyboard)            
        Yen             =0x7D,    // (Japanese keyboard)            
        ABNT_C2         =0x7E,    // Numpad . on Portugese (Brazilian) keyboards 
        NumpadEquals    =0x8D,    // = on numeric keypad (NEC PC98) 
        PrevTrack       =0x90,    // Previous Track (KC_CIRCUMFLEX on Japanese keyboard) 
        At              =0x91,    //                     (NEC PC98) 
        Colon           =0x92,    //                     (NEC PC98) 
        Underline       =0x93,    //                     (NEC PC98) 
        Kanji           =0x94,    // (Japanese keyboard)            
        Stop            =0x95,    //                     (NEC PC98) 
        AX              =0x96,    //                     (Japan AX) 
        Unlabeled       =0x97,    //                        (J3100) 
        NextTrack       =0x99,    // Next Track 
        NumpadEnter     =0x9C,    // Enter on numeric keypad 
        RightControl    =0x9D,
        Mute            =0xA0,    // Mute 
        Calculator      =0xA1,    // Calculator 
        PlayPause       =0xA2,    // Play / Pause 
        MediaStop       =0xA4,    // Media Stop 
        VolumeDown      =0xAE,    // Volume - 
        VolumeUp        =0xB0,    // Volume + 
        WebHome         =0xB2,    // Web home 
        NumpadComma     =0xB3,    // , on numeric keypad (NEC PC98) 
        Divide          =0xB5,    // / on numeric keypad 
        SysRq           =0xB7,
        RightAlt        =0xB8,    // right Alt 
        Pause           =0xC5,    // Pause 
        Home            =0xC7,    // Home on arrow keypad 
        ArrowUp         =0xC8,    // UpArrow on arrow keypad 
        PageUp          =0xC9,    // PgUp on arrow keypad 
        ArrowLeft       =0xCB,    // LeftArrow on arrow keypad 
        ArrowRight      =0xCD,    // RightArrow on arrow keypad 
        End             =0xCF,    // End on arrow keypad 
        ArrowDown       =0xD0,    // DownArrow on arrow keypad 
        PageDown		=0xD1,    // PgDn on arrow keypad 
        Insert          =0xD2,    // Insert on arrow keypad 
        Delete          =0xD3,    // Delete on arrow keypad 
        LeftWindows     =0xDB,    // Left Windows key 
        RightWindow     =0xDC,    // Right Windows key 
        RightWindows    =0xDC,    // Right Windows key - Correct spelling :) 
        AppMenu         =0xDD,    // AppMenu key 
        Power           =0xDE,    // System Power 
        Sleep           =0xDF,    // System Sleep 
        Wake			=0xE3,    // System Wake 
        WebSearch		=0xE5,    // Web Search 
        WebFavorites	=0xE6,    // Web Favorites 
        WebRefresh		=0xE7,    // Web Refresh 
        WebStop			=0xE8,    // Web Stop 
        WebForward		=0xE9,    // Web Forward 
        WebBack			=0xEA,    // Web Back 
        MyComputer		=0xEB,    // My Computer 
        Mail			=0xEC,    // Mail 
        MediaSelect		=0xED     // Media Select 
    };
*/

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
