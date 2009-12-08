#include "Common.h"

#include "GUIMgr.h"
#include "ResourceProvider.h"

#include "InputSystem/InputMgr.h"
#include "InputSystem/InputActions.h"

#include "CEGUI.h"
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"

namespace GUISystem
{
	/// @name Prototypes for utility functions
	//@{
	CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key);
	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn);
	//@}

	GUIMgr::GUIMgr():
		mCegui(0),
		mCurrentWindowRoot(0),
		mRenderer(0),
		mResourceProvider(0),
		mConsoleIsLoaded(false)
	{
		ocInfo << "*** GUIMgr init ***";

		mRenderer = &CEGUI::OpenGLRenderer::create();
		mResourceProvider = new ResourceProvider();

		mCegui = &CEGUI::System::create(*mRenderer, mResourceProvider);

		gInputMgr.AddInputListener(this);
		CEGUI::Imageset::setDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	}

	GUIMgr::~GUIMgr()
	{
		gInputMgr.RemoveInputListener(this);

		CEGUI::System::destroy();
		CEGUI::OpenGLRenderer::destroy(*mRenderer);
		delete mResourceProvider;
		//delete mCurrentWindowRoot;
	}



#if 0 // TODO
	void GUIMgr::LoadGUI() {
		ocInfo << "GUI Menu init";
		//CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme");
		CEGUI::SchemeManager::getSingleton().loadScheme( "BGGUI.scheme");

		// load in a font.  The first font loaded automatically becomes the default font.
		if( !CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
		  CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );

		mCegui->setDefaultFont( "Commonwealth-10" );
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );

		string layout = GlobalProperties::Get<Core::Config>("GlobalConfig").GetString("Layout", "Battleships.layout", "CEGUI");

		CEGUI::Window* CurrentWindowRoot =
			CEGUI::WindowManager::getSingleton().loadWindowLayout( layout.c_str() );

		CEGUI::System::getSingleton().setGUISheet( CurrentWindowRoot );
		RegisterEvents();

	}
#endif

	void GUIMgr::LoadStyle( void )
	{
		ocInfo << "*** GUIMgr load style ***";

		gResourceMgr.AddResourceDirToGroup("gui/schemes", "schemes", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup("gui/imagesets", "imagesets", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup("gui/fonts", "fonts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup("gui/layouts", "layouts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup("gui/looknfeel", "looknfeels", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);

		CEGUI::SchemeManager::getSingleton().create("Lightweight.scheme");

		if (!CEGUI::FontManager::getSingleton().isDefined("Commonwealth-10"))
			CEGUI::FontManager::getSingleton().create("Commonwealth-10.font");

		CEGUI::SchemeManager::getSingleton().create("Console.scheme");
		mCurrentWindowRoot = CEGUI::WindowManager::getSingleton().loadWindowLayout("Console.layout");
		CEGUI::System::getSingleton().setGUISheet(mCurrentWindowRoot);
		mConsoleIsLoaded = true;

		mCegui->setDefaultFont("Commonwealth-10");
		mCegui->setDefaultMouseCursor("Lightweight", "MouseArrow");
		RegisterEvents();
	}



	void GUIMgr::RenderGUI() const
	{
		OC_DASSERT(mCegui);
		mCegui->renderGUI();
	}

	void GUIMgr::Update(float32 delta)
	{
		OC_DASSERT(mCegui);
		mCegui->injectTimePulse(delta);
		/// @note: For some reason we injected mouse position here, instead of in
		///        MouseMoved(). Delete this comment after you are sure that everything
		///        is working well.
	}

	void GUIMgr::KeyPressed(const InputSystem::KeyInfo& ke)
	{
		OC_DASSERT(mCegui);

		// ToggleConsole button is hardwired here.
		if (ke.keyAction == InputSystem::KC_GRAVE) {
			ToggleConsole();
			return;
		}

		if (!mCegui->injectKeyDown(KeyMapperOIStoCEGUI(ke.keyAction)))
		{
			// If the key was not processed by the gui system,
			// inject corresponding character.
			mCegui->injectChar(ke.keyCode);
		}
	}

	void GUIMgr::KeyReleased(const InputSystem::KeyInfo& ke)
	{
		OC_DASSERT(mCegui);
		mCegui->injectKeyUp(KeyMapperOIStoCEGUI(ke.keyAction));
	}

	void GUIMgr::MouseMoved(const InputSystem::MouseInfo& mi)
	{
		OC_DASSERT(mCegui);
		mCegui->injectMouseWheelChange(float(mi.wheelDelta));
		mCegui->injectMousePosition(float(mi.x), float(mi.y));
	}

	void GUIMgr::MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
	{
		OC_DASSERT(mCegui);
		mCegui->injectMouseButtonDown(ConvertMouseButtonEnum(btn));
	}

	void GUIMgr::MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
	{
		OC_DASSERT(mCegui);
		mCegui->injectMouseButtonUp(ConvertMouseButtonEnum(btn));
	}

	void GUIMgr::AddConsoleListener(IConsoleListener* listener)
	{
		mConsoleListeners.push_back(listener);
	}

	void GUIMgr::AddConsoleMessage(string message, const GfxSystem::Color& color)
	{
		if (!mConsoleIsLoaded)
		{
			ocWarning << "Trying to write to game console, but console is not loaded. Message: " << message;
			return;
		}
#if 0 // TODO
		CEGUI::Listbox* pane = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/Pane");

		CEGUI::ListboxTextItem* new_item = new CEGUI::ListboxTextItem(message);
		new_item->setTextColours(CEGUI::colour( color.GetARGB() ) );

		pane->addItem(new_item);
		pane->ensureItemIsVisible(new_item);

		uint32 item_count;
		while ((item_count = pane->getItemCount()) > 50)
			pane->removeItem(pane->getListboxItemFromIndex(item_count - 1));

		set<IConsoleListener*>::iterator iter = mConsoleListeners.begin();
		while (iter != mConsoleListeners.end())
		{
			(*iter)->EventConsoleCommand(message);
			++iter;
		}
	}
#endif
}




	void GUIMgr::RegisterEvents()
	{
		OC_DASSERT(mCegui);

		CEGUI::Window* console = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		console->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&GUIMgr::ConsoleCommandEvent, this));
	}


#if 0
	bool GUIMgr::QuitEvent(const CEGUI::EventArgs& e) {
		gApp.RequestStateChange(Core::AS_SHUTDOWN);
		return true;
	}
#endif

	bool GUIMgr::ConsoleCommandEvent(const CEGUI::EventArgs& e)
	{
		///@todo Implement me!
#if 0
		CEGUI::Window* prompt = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
		string message(prompt->getText().c_str());
		if (message == "quit")
			gApp.RequestStateChange( Core::AS_SHUTDOWN );
		if ( message.length() >= 9 ) {
			string prefix = message.substr(0, 7);
			string args = message.substr(8);
			if (prefix == "addtext")
				AddStaticText( 0.0f, 0.0f, "test text", args, GfxSystem::Color( 255, 0, 0),
					ANCHOR_BOTTOM | ANCHOR_RIGHT,
					ANCHOR_BOTTOM | ANCHOR_RIGHT );
		}
		AddLastCommand(message);
		AddConsoleMessage(message);
		prompt->setText("");
#endif
		return true;
	}

	void GUIMgr::ToggleConsole()
	{
		try
		{
			CEGUI::Window* Root = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot");
			Root->setVisible( !Root->isVisible() );
			CEGUI::Window* Console = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
			Console->activate();
		}
		catch(const CEGUI::UnknownObjectException&)
		{
			OC_ASSERT_MSG(false, "Could not toggle console - necessary GUI components not found.");
		}
	}

#if 0
	void GUIMgr::AddLastCommand(string command)
	{
		if (mLastCommands.size() == 25)
			mLastCommands.pop_back();
		mLastCommands.push_front( command );
		mCurrentLastSelected = mLastCommands.begin();
	}

	void GUIMgr::LoadLastCommand() {
		if (mLastCommands.size() == 0)
			return;
		if (mCurrentLastSelected == mLastCommands.end()) {
			mCurrentLastSelected = mLastCommands.begin();
		}

		CEGUI::Editbox* editbox = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");

		editbox->setText(*mCurrentLastSelected);
	}

	Vector2 GUIMgr::GetTextSize( const string & text, const string & fontid ) {
		CEGUI::Font* font;
		if (fontid != "")
			font = CEGUI::FontManager::getSingleton().getFont(fontid);
		else
			font = CEGUI::System::getSingleton().getDefaultFont();
		return Vector2(font->getTextExtent(text)/gGfxWindow.GetResolutionWidth(),
			font->getFontHeight()/gGfxWindow.GetResolutionHeight());
	}




	void GUIMgr::AddStaticText( float32 x, float32 y, const string & id, const string & text,
			const GfxSystem::Color color/* = GfxSystem::Color(255,255,255)*/,
			uint8 text_anchor/* = ANCHOR_LEFT | ANCHOR_TOP*/,
			uint8 screen_anchor/* = ANCHOR_LEFT | ANCHOR_TOP*/,
			const string & fontid )
	{
		map<string, StaticElement*>::iterator iter = mCreatedStaticElements.find( id );
		if (iter == mCreatedStaticElements.end()) {
			StaticText* ptr = new StaticText( x, y, id, text, color, text_anchor, screen_anchor, fontid );
			mCreatedStaticElements.insert( Containers::make_pair( id, (StaticElement*)ptr ) );
		} else {
			StaticText* static_text = (StaticText*)iter->second;
			static_text->SetStaticText( x, y, text, color, text_anchor, screen_anchor, fontid );
		}
	}

	StaticText* GUIMgr::GetStaticText( const string & id ) {
		map<string, StaticElement*>::iterator iter = mCreatedStaticElements.find( id );
		return (StaticText*)(iter->second);
	}

#endif

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
		case InputSystem::MBTN_UNKNOWN:
			break;
		}
		return CEGUI::LeftButton;
	}
}
