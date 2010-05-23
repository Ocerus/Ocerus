#include "Common.h"

#include "GUIMgr.h"
#include "ResourceProvider.h"
#include "GUIConsole.h"
#include "CEGUITools.h"

#include "Core/Application.h"

#include "InputSystem/InputMgr.h"
#include "InputSystem/InputActions.h"

#include "Editor/EditorMgr.h"

#include "CEGUI.h"
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"

#include "ViewportWindow.h"

namespace GUISystem
{
	/// @name Prototypes for utility functions
	//@{
	CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key);
	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn);
	bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata);
	//@}
	
	const StringKey GUIMgr::GUIGroup = "GUI";

	GUIMgr::GUIMgr():
		mCegui(0),
		mWindowRoot(0),
		mCurrentRootLayout(0),
		mGUIConsole(0),
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

		mGUIConsole = new GUIConsole();
	}

	GUIMgr::~GUIMgr()
	{
		delete mGUIConsole;

		gGfxWindow.RemoveScreenListener(this);
		gInputMgr.RemoveInputListener(this);


		CEGUI::System::destroy();
		CEGUI::OpenGLRenderer::destroy(*mRenderer);
		delete mResourceProvider;
	}

	void GUIMgr::Init()
	{
		/// Register CEGUI resources.
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/schemes", "schemes", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/imagesets", "imagesets", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/fonts", "fonts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/layouts", "layouts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/looknfeel", "looknfeels", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);

		CEGUI_EXCEPTION_BEGIN
		{
			/// Register custom Window subclasses to the CEGUI
			CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<ViewportWindow> >();
			

			///@todo Improve GUI scheme loading. For example Editor.scheme does not need to be loaded unless in editor mode.
			CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
			CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
			CEGUI::SchemeManager::getSingleton().create("Editor.scheme");

			/// Create and set root widget
			mWindowRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
			mCegui->setGUISheet(mWindowRoot);
			mWindowRoot->setMousePassThroughEnabled(true);

			/// Set defaults
			mCegui->setDefaultFont("DejaVuSans-10");
			mCegui->setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
			//mCegui->setMouseClickEventGenerationEnabled(false);
		}
		CEGUI_EXCEPTION_END_CRITICAL

		mGUIConsole->Init();
	}

	void GUIMgr::Deinit()
	{
		mGUIConsole->Deinit();

		CEGUI::WindowManager::getSingleton().destroyAllWindows();
	}

	void GUIMgr::DisconnectEvent( const CEGUI::Event::Connection eventConnection )
	{
		mDeadEventConnections.push_back(eventConnection);
	}

	void GUIMgr::ProcessDisconnectedEventList()
	{
		for (list<CEGUI::Event::Connection>::iterator it=mDeadEventConnections.begin(); it!=mDeadEventConnections.end(); ++it)
		{
			(*it)->disconnect();
		}
		mDeadEventConnections.clear();
	}

	bool GUIMgr::LoadRootLayout(const string& filename)
	{
		OC_DASSERT(mCegui);
		UnloadRootLayout();
		CEGUI::Window* newLayout = LoadWindowLayout(filename);
		if (!newLayout) return false;
		mCurrentRootLayout = newLayout;
		mWindowRoot->addChildWindow(mCurrentRootLayout);
		return true;
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

	bool GUIMgr::KeyPressed(const InputSystem::KeyInfo& ke)
	{
		OC_DASSERT(mCegui);
		mCurrentInputEvent.keyInfo = &ke;

		// ToggleConsole button is hardwired here.
		if (ke.keyCode == InputSystem::KC_GRAVE) {
			mGUIConsole->ToggleConsole();
			return true;
		}

		if (!mCegui->injectKeyDown(KeyMapperOIStoCEGUI(ke.keyCode)))
		{
			// If the key was not processed by the gui system, try to handle hotkeys.
			if (ke.keyCode == InputSystem::KC_ESCAPE)
			{
				gApp.Shutdown();
				return true;
			}

			// Finally inject corresponding character.
			return mCegui->injectChar(ke.charCode);
		}

		return false;
	}

	bool GUIMgr::KeyReleased(const InputSystem::KeyInfo& ke)
	{
		OC_DASSERT(mCegui);
		mCurrentInputEvent.keyInfo = &ke;
		return mCegui->injectKeyUp(KeyMapperOIStoCEGUI(ke.keyCode));
	}

	bool GUIMgr::MouseMoved(const InputSystem::MouseInfo& mi)
	{
		OC_DASSERT(mCegui);
		mCurrentInputEvent.mouseInfo = &mi;
		bool wheelInjected = mCegui->injectMouseWheelChange(float(mi.wheelDelta));
		bool positionInjected = mCegui->injectMousePosition(float(mi.x), float(mi.y));
		return wheelInjected || positionInjected;
	}

	bool GUIMgr::MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
	{
		OC_UNUSED(mi);
		OC_DASSERT(mCegui);
		mCurrentInputEvent.mouseInfo = &mi;
		mCurrentInputEvent.mouseButton = btn;
		bool result = mCegui->injectMouseButtonDown(ConvertMouseButtonEnum(btn));
		return result;
	}

	bool GUIMgr::MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
	{
		OC_UNUSED(mi);
		OC_DASSERT(mCegui);
		mCurrentInputEvent.mouseInfo = &mi;
		mCurrentInputEvent.mouseButton = btn;
		gEditorMgr.EnablePopupClosing();
		bool result = mCegui->injectMouseButtonUp(ConvertMouseButtonEnum(btn));
		gEditorMgr.CloseAllPopupMenus();
		return result;
	}

	void GUIMgr::ResolutionChanged(const uint32 width, const uint32 height)
	{
		OC_DASSERT(mCegui);
		return mCegui->notifyDisplaySizeChanged(CEGUI::Size((float32)width, (float32)height));
	}

/*
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
*/
#if 0
	bool GUIMgr::QuitEvent(const CEGUI::EventArgs& e) {
		gApp.RequestStateChange(Core::AS_SHUTDOWN);
		return true;
	}
#endif


#if 0
bool GUIMgr::ConsoleCommandEvent(const CEGUI::EventArgs& e)
	{
		OC_DASSERT(mConsoleIsLoaded);
		string message(mConsolePrompt->getText().c_str());


		if ( message.length() >= 9 ) {
			string prefix = message.substr(0, 7);
			string args = message.substr(8);
			if (prefix == "addtext")
				AddStaticText( 0.0f, 0.0f, "test text", args, GfxSystem::Color( 255, 0, 0),
					ANCHOR_BOTTOM | ANCHOR_RIGHT,
					ANCHOR_BOTTOM | ANCHOR_RIGHT );
		}
		//AddLastCommand(message);
		AddConsoleMessage(message);
		mConsolePrompt->setText("");
		return true;
	}
	#endif


	CEGUI::Window* GUIMgr::LoadWindowLayout(const string& filename, const string& name_prefix, const string& resourceGroup)
	{
		CEGUI::Window* result = 0;
		CEGUI_EXCEPTION_BEGIN
		{
			result = CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, name_prefix, resourceGroup, PropertyCallback);
		}
		CEGUI_EXCEPTION_END
		return result;
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

	/// @todo Resolve duplicate of this constant in CEGUITools.cpp
	
	/**
	 * The PropertyCallback function is a callback used to translate textual data
	 * from window layout. This callback should be used when loading window layouts
	 * in CEGUI::WindowManager::loadWindowLayout.
	 */
	bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
	{
		/// @todo Resolve duplicate of this function in CEGUITools.cpp
		OC_UNUSED(userdata);
	  if ((propname == "Text" || propname == "Tooltip") &&
		  propvalue.size() > 2 &&
		  propvalue.at(0) == '$' &&
		  propvalue.at(propvalue.size() - 1) == '$')
	  {
		  /// Use StringMgr to translate textual data in GUI.
		  CEGUI::String translatedText = gStringMgrSystem.GetTextData(GUIMgr::GUIGroup, propvalue.substr(1, propvalue.size() - 2).c_str());
		  window->setProperty(propname, translatedText);
		  return false;
	  }
	  return true;
	}
}

