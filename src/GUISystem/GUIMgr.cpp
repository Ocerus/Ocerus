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
	bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata);
	//@}

	GUIMgr::GUIMgr():
		mCegui(0),
		mWindowRoot(0),
		mCurrentRootLayout(0),
		mConsoleRoot(0),
		mConsolePrompt(0),
		mConsoleMessages(0),
		mRenderer(0),
		mResourceProvider(0),
		mConsoleIsLoaded(false)
	{
		ocInfo << "*** GUIMgr init ***";

		mRenderer = &CEGUI::OpenGLRenderer::create();
		mResourceProvider = new ResourceProvider();

		mCegui = &CEGUI::System::create(*mRenderer, mResourceProvider);

		gInputMgr.AddInputListener(this);
		gGfxWindow.AddScreenListener(this);

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
		try
		{
			gResourceMgr.AddResourceDirToGroup("gui/schemes", "schemes", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
			gResourceMgr.AddResourceDirToGroup("gui/imagesets", "imagesets", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
			gResourceMgr.AddResourceDirToGroup("gui/fonts", "fonts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
			gResourceMgr.AddResourceDirToGroup("gui/layouts", "layouts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
			gResourceMgr.AddResourceDirToGroup("gui/looknfeel", "looknfeels", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);

			///@todo Improve GUI scheme loading. For example Editor.scheme does not need to be loaded unless in editor mode.
			CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
			CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
			CEGUI::SchemeManager::getSingleton().create("Editor.scheme");

			if (!CEGUI::FontManager::getSingleton().isDefined("Commonwealth-10"))
				CEGUI::FontManager::getSingleton().create("Commonwealth-10.font");

			CEGUI::SchemeManager::getSingleton().create("Console.scheme");
			mWindowRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
			CEGUI::System::getSingleton().setGUISheet(mWindowRoot);


			mCegui->setDefaultFont("DejaVuSans-10");
			mCegui->setDefaultMouseCursor("TaharezLook", "MouseArrow");

			InitConsole();

			/// -------------------------
			/// Showing HelloWorld window
			//CEGUI::Window* helloWorldWindow = LoadWindowLayout("HelloWorld.layout");
			//mWindowRoot->addChildWindow(helloWorldWindow);
			/// -------------------------

		}
		catch (CEGUI::Exception& exception)
		{
			ocWarning << "CEGUI exception caught (" << exception.getName() << "): " << exception.getMessage();
		}
	}

	void GUIMgr::LoadRootLayout(const string& filename)
	{
		OC_DASSERT(mCegui);
		UnloadRootLayout();
		mCurrentRootLayout = LoadWindowLayout(filename);
		mWindowRoot->addChildWindow(mCurrentRootLayout);
	}

	void GUIMgr::UnloadRootLayout()
	{
		OC_DASSERT(mCegui);
		if (mCurrentRootLayout != 0)
		{
			mWindowRoot->removeChildWindow(mCurrentRootLayout);
			CEGUI::WindowManager::getSingleton().destroyWindow(mCurrentRootLayout);
			mCurrentRootLayout = 0;
		}
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

	void GUIMgr::ResolutionChanged(int32 width, int32 height)
	{
		OC_DASSERT(mCegui);
		mCegui->notifyDisplaySizeChanged(CEGUI::Size((float32)width, (float32)height));
	}

	void GUIMgr::AddConsoleListener(IConsoleListener* listener)
	{
		mConsoleListeners.push_back(listener);
	}

	void GUIMgr::AddConsoleMessage(string message, const GfxSystem::Color& color)
	{
		if (!mConsoleIsLoaded)
		{
			ocWarning << "AddConsoleMessage: " << message;
			return;
		}

		CEGUI::ListboxTextItem* new_item = new CEGUI::ListboxTextItem(message);
		new_item->setTextColours(CEGUI::colour( color.GetARGB() ) );

		mConsoleMessages->addItem(new_item);
		mConsoleMessages->ensureItemIsVisible(new_item);

		uint32 item_count;
		while ((item_count = mConsoleMessages->getItemCount()) > 50)
			mConsoleMessages->removeItem(mConsoleMessages->getListboxItemFromIndex(item_count - 1));

		vector<IConsoleListener*>::iterator iter = mConsoleListeners.begin();
		while (iter != mConsoleListeners.end())
		{
			(*iter)->EventConsoleCommand(message);
			++iter;
		}
	}

	void GUIMgr::WriteLogMessageToConsole(const string& msg, int32 loggingLevel)
	{
		if (!mConsoleIsLoaded) return;
		AddConsoleMessage(msg);
	}

#if 0
	bool GUIMgr::QuitEvent(const CEGUI::EventArgs& e) {
		gApp.RequestStateChange(Core::AS_SHUTDOWN);
		return true;
	}
#endif

	void GUIMgr::InitConsole()
	{
		OC_DASSERT(mCegui);
		OC_DASSERT(mConsoleRoot == 0);
		OC_DASSERT(mConsolePrompt == 0);
		OC_DASSERT(mConsoleMessages == 0);
		try
		{
			mConsoleRoot = LoadWindowLayout("Console.layout");
			mWindowRoot->addChildWindow(mConsoleRoot);
			mConsolePrompt = CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/ConsolePrompt");
			mConsoleMessages = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("ConsoleRoot/Pane");
			mConsolePrompt->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
				CEGUI::Event::Subscriber(&GUIMgr::ConsoleCommandEvent, this));
			mConsoleIsLoaded = true;
		}
		catch(const CEGUI::UnknownObjectException&)
		{
			ocError << "Could not initialize console - necessary GUI components not found.";
		}
	}

	bool GUIMgr::ConsoleCommandEvent(const CEGUI::EventArgs& e)
	{
		OC_DASSERT(mConsoleIsLoaded);
		string message(mConsolePrompt->getText().c_str());

#if 0
		if ( message.length() >= 9 ) {
			string prefix = message.substr(0, 7);
			string args = message.substr(8);
			if (prefix == "addtext")
				AddStaticText( 0.0f, 0.0f, "test text", args, GfxSystem::Color( 255, 0, 0),
					ANCHOR_BOTTOM | ANCHOR_RIGHT,
					ANCHOR_BOTTOM | ANCHOR_RIGHT );
		}
#endif
		//AddLastCommand(message);
		AddConsoleMessage(message);
		mConsolePrompt->setText("");
		return true;
	}

	void GUIMgr::ToggleConsole()
	{
		if (!mConsoleIsLoaded)
			return;

		mConsoleRoot->setVisible(!mConsoleRoot->isVisible());
		mConsolePrompt->activate();
	}

	CEGUI::Window* GUIMgr::LoadWindowLayout(const string& filename, const string& name_prefix, const string& resourceGroup)
	{
		return CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, name_prefix, resourceGroup, PropertyCallback);
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

	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn)
	{
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

	/**
	 * The PropertyCallback function is a callback used to translate textual data
	 * from window layout. This callback should be used when loading window layouts
	 * in CEGUI::WindowManager::loadWindowLayout.
	 */
	bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
	{
		if (propname == "Text" &&
			propvalue.size() > 2 &&
			propvalue.at(0) == '$' &&
			propvalue.at(propvalue.size() - 1) == '$')
		{
			/// @todo Use StringMgr to translate textual data in GUI.
			CEGUI::String translatedText = "_" + propvalue.substr(1, propvalue.size() - 2);
			window->setProperty(propname, translatedText);
			return false;
		}
		return true;
	}
}
