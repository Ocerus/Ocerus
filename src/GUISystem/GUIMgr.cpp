#include "Common.h"

#include "GUIMgr.h"
#include "ResourceProvider.h"
#include "ScriptProvider.h"
#include "GUIConsole.h"
#include "CEGUITools.h"

#include "Core/Application.h"

#include "InputSystem/InputMgr.h"
#include "InputSystem/InputActions.h"

#include "Editor/EditorMgr.h"

#include "CEGUI.h"
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"

#include "ViewportWindow.h"

#undef CreateWindow
#undef CreateWindowA
#undef CreateWindowW

/// WTF IS CEGUI::WindowManager::getSingleton().recreateWindow()
#ifdef __WIN__

/// If defined the CEGUI windows will be recycled using a system of caches.
#define RECYCLE_WINDOWS


#endif

using namespace GUISystem;

/// @name Prototypes for utility functions
//@{
/// Converts OIS key codes to CEGUI equivalents.
CEGUI::uint KeyMapperOIStoCEGUI(InputSystem::eKeyCode key);

/// Convert OIS mouse button enum to CEGUI equivalent.
CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn);

/// The callback function that is called on every property loaded from a system layout file.
bool SystemLayoutPropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata);

/// The callback function that is called on every property loaded from a project layout file.
bool ProjectLayoutPropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata);
//@}

const StringKey GUIMgr::GUIGroup = "GUI";

GUIMgr::GUIMgr():
	mCegui(0),
	mCurrentRootLayout(0),
	mGUIConsole(0),
	mRenderer(0),
	mResourceProvider(0)
{
	ocInfo << "*** GUIMgr init ***";

	mRenderer = &CEGUI::OpenGLRenderer::create();
	mResourceProvider = new ResourceProvider();
	mScriptProvider = new ScriptProvider();
	mCegui = &CEGUI::System::create(*mRenderer, mResourceProvider, 0, 0, mScriptProvider);

	gInputMgr.AddInputListener(this);
	gGfxWindow.AddScreenListener(this);

	CEGUI::Imageset::setDefaultResourceGroup("gui-imagesets");
	CEGUI::Font::setDefaultResourceGroup("gui-fonts");
	CEGUI::Scheme::setDefaultResourceGroup("gui-schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("gui-looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("gui-layouts");
	mGUIConsole = new GUIConsole();
}

GUIMgr::~GUIMgr()
{
	delete mGUIConsole;
	mGUIConsole = 0;

	gGfxWindow.RemoveScreenListener(this);
	gInputMgr.RemoveInputListener(this);

	ocInfo << "Destroying CEGUI";
	CEGUI::System::destroy();
	CEGUI::OpenGLRenderer::destroy(*mRenderer);
	ocInfo << "CEGUI destroyed";

	delete mResourceProvider;
	delete mScriptProvider;
}

void GUIMgr::Init()
{
	/// Register CEGUI system resources.
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/schemes", "gui-schemes", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/imagesets", "gui-imagesets", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/fonts", "gui-fonts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/layouts", "gui-layouts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/looknfeel", "gui-looknfeels", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);

	CEGUI_EXCEPTION_BEGIN
	{
		// Register custom Window subclasses to the CEGUI
		CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<ViewportWindow> >();

		///@todo Improve GUI scheme loading. For example Editor.scheme does not need to be loaded unless in editor mode.
		CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
		CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
		CEGUI::SchemeManager::getSingleton().create("Editor.scheme");

		// Set defaults
		mCegui->setDefaultFont("DejaVuSans-10");
		mCegui->setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
		mCegui->setMouseClickEventGenerationEnabled(true);
	}
	CEGUI_EXCEPTION_END_CRITICAL
}

void GUIMgr::Deinit()
{
	DeinitConsole();
	ClearWindowCaches();
	CEGUI::WindowManager::getSingleton().destroyAllWindows();
}

void GUIMgr::InitConsole()
{
	mGUIConsole->Init();
}

void GUIMgr::DeinitConsole()
{
	mGUIConsole->Deinit();
}

void GUIMgr::LoadSystemScheme(const string& filename)
{
	OC_DASSERT(mCegui);
	CEGUI::SchemeManager::getSingleton().create(filename);
}

void GUIMgr::LoadProjectScheme(const string& filename)
{
	OC_DASSERT(mCegui);
	/// @todo resource groups
	CEGUI::SchemeManager::getSingleton().create(filename, "project");
}

CEGUI::Window* GUIMgr::LoadSystemLayout(const CEGUI::String& filename, const CEGUI::String& namePrefix)
{
	OC_DASSERT(mCegui);
	CEGUI::Window* resultLayout = 0;
	CEGUI_EXCEPTION_BEGIN
	{
		resultLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, namePrefix, "", SystemLayoutPropertyCallback);
	}
	CEGUI_EXCEPTION_END

		if (resultLayout) ocInfo << "System GUI layout " << filename << " loaded.";
		else ocWarning << "Cannot load system GUI layout " << filename << ".";

		return resultLayout;
}

CEGUI::Window* GUIMgr::LoadProjectLayout(const CEGUI::String& filename, const CEGUI::String& namePrefix)
{
	OC_DASSERT(mCegui);
	CEGUI::Window* resultLayout = 0;
	CEGUI_EXCEPTION_BEGIN
	{
		resultLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, namePrefix, "Project", ProjectLayoutPropertyCallback);
	}
	CEGUI_EXCEPTION_END

		if (resultLayout) ocInfo << "Project GUI layout " << filename << " loaded.";
		else ocWarning << "Cannot load project GUI layout " << filename << ".";

		return resultLayout;
}

bool GUIMgr::SetGUISheet(CEGUI::Window* sheet)
{
	OC_DASSERT(mCegui);
	if (sheet)
	{
		mCegui->setGUISheet(sheet);
		sheet->setMousePassThroughEnabled(true);
		ocInfo << "GUI sheet set to " << sheet->getName() << ".";
		return true;
	}
	else
	{
		return false;
	}
}

CEGUI::Window* GUIMgr::GetGUISheet() const
{
	OC_DASSERT(mCegui);
	return mCegui->getGUISheet();
}

void GUIMgr::DestroyWindow(CEGUI::Window* window)
{
	if (!window) return;

#ifdef RECYCLE_WINDOWS

	DestroyWindowChildren(window);
	if (window->getParent()) window->getParent()->removeChildWindow(window); // this is the most time consuming action here!!!
	window->hide();

	WindowTypeCache* cache = mWindowCache[window->getType().c_str()];
	if (cache && window->isUserStringDefined("CreatedByGuiMgr"))
	{
		if (cache->count == cache->list.size())
		{
			size_t firstIndex = cache->list.size();
			cache->list.resize(cache->list.size() + 200);
			for (size_t i=firstIndex; i<cache->list.size(); ++i) cache->list[i] = 0;
		}
		cache->list[cache->count++] = window;
		ocTrace << "Destroyed window " << window->getType() << " " << window->getName() << " " << window << "; " << window->getChildCount() << " children";
	}
	else
	{
		OC_FAIL("This shouldn't happen. Windows not created by us should be destroyed using DestroyWindowDirectly().");
		ocWarning << "Destroying CEGUI window " << window->getType() << " " << window->getName() << " " << window;
		DestroyWindowDirectly(window);
	}

#else
	return DestroyWindowDirectly(window);
#endif
}

void GUISystem::GUIMgr::DestroyWindowChildren( CEGUI::Window* window )
{
	if (!window) return;

	for (int32 i=window->getChildCount()-1; i>=0; --i)
	{
		if (window->getChildAtIdx(i)->isUserStringDefined("CreatedByGuiMgr"))
		{
			// now we know this window was created by us, so it wasn't created automatically
			DestroyWindow(window->getChildAtIdx(i));
		}
	}
}

CEGUI::Window* GUIMgr::CreateWindow( const string& type, bool reallocateOnHeap )
{
#ifdef RECYCLE_WINDOWS

	if (mWindowCache.find(type) == mWindowCache.end()) InitWindowCache(type);
	
	WindowTypeCache* cache = mWindowCache[type];
	OC_ASSERT(cache);

	if (cache->count == 0)
	{
		ocWarning << "Cache too small; creating CEGUI window " << type << " directly";
		cache->list[cache->count++] = CreateWindowDirectly(type);
	}

	--cache->count;
	CEGUI::Window* window = cache->list[cache->count];
	cache->list[cache->count] = 0;
	OC_ASSERT(window);

	if (reallocateOnHeap)
	{
		window = CEGUI::WindowManager::getSingleton().recreateWindow(window);
		window->setDestroyedByParent(false); // to prevent CEGUI from destroying our own windows
		window->setUserString("CreatedByGuiMgr", "True"); // mark this window is ours
	}
	else
	{
		window->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0, 0));
		if (window->isPropertyPresent("ReadOnly")) window->setProperty("ReadOnly", "False");
		window->show();
	}
	

	ocTrace << "Created window " << window->getType() << " " << window->getName() << "; " << window->getChildCount() << " children";

	return window;

#else
	return CreateWindowDirectly(type);
#endif
}

CEGUI::Window* GUIMgr::CreateWindowDirectly( const string& type )
{
	static uint64 windowID = 0;
	CEGUI::Window* window = CEGUI::WindowManager::getSingleton().createWindow(type, "EngineCreated_" + StringConverter::ToString(windowID++));
#ifdef RECYCLE_WINDOWS
	window->setDestroyedByParent(false); // to prevent CEGUI from destroying our own windows
#endif
	window->setUserString("CreatedByGuiMgr", "True"); // mark this window is ours
	return window;
}

void GUISystem::GUIMgr::InitWindowCache( const string& type )
{
	const size_t cacheSize = 300;
	ocInfo << "Creating a cache for " << cacheSize << " GUI windows " << type;

	WindowTypeCache* cache = new WindowTypeCache();
	cache->list.resize(2*cacheSize);
	cache->count = cacheSize;

	for (size_t i=0; i<cache->list.size(); ++i) cache->list[i] = 0;

	for (size_t i=0; i<cache->count; ++i)
	{
		cache->list[i] = CreateWindowDirectly(type);
	}

	mWindowCache[type] = cache;
}

void GUISystem::GUIMgr::ClearWindowCaches()
{
	for (WindowMap::iterator it=mWindowCache.begin(); it!=mWindowCache.end(); ++it)
	{
		WindowTypeCache* cache = it->second;
		OC_ASSERT(cache);
		for (size_t i=0; i<cache->count; ++i)
		{
			CEGUI::WindowManager::getSingleton().destroyWindow(cache->list[i]);
		}
		delete cache;
	}
	mWindowCache.clear();
}

CEGUI::Window* GUISystem::GUIMgr::GetWindow( const string& name )
{
	return CEGUI::WindowManager::getSingleton().getWindow(name.c_str());
}

bool GUISystem::GUIMgr::WindowExists( const string& name )
{
	return CEGUI::WindowManager::getSingleton().isWindowPresent(name.c_str());
}

void GUISystem::GUIMgr::DestroyWindowDirectly( CEGUI::Window* window )
{
	CEGUI::WindowManager::getSingleton().destroyWindow(window);
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

void GUIMgr::RenderGUI() const
{
	OC_DASSERT(mCegui);
	mCegui->renderGUI();
}

void GUIMgr::Update(float32 delta)
{
	OC_DASSERT(mCegui);
	mCegui->injectTimePulse(delta);
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

	bool wheelInjected = false;
	if (mi.wheelDelta != 0)
	{
		CEGUI::Window* wheelTarget = CEGUI::System::getSingleton().getWindowContainingMouse();

		// save the currently active window
		CEGUI::Window* root = CEGUI::System::getSingleton().getGUISheet();
		CEGUI::Window* prevActive = root->getActiveChild();

		// find the first window wanting the mouse wheel
		while(wheelTarget)
		{
			if( wheelTarget->isPropertyPresent("WantsMouseWheel") 
				&& wheelTarget->getProperty("WantsMouseWheel") == "True"
				&& wheelTarget->isVisible()
				&& !wheelTarget->isDisabled())
			{   
				break;
			}

			wheelTarget = wheelTarget->getParent();                    
		}

		// capture input for the wheel target for a while so that its child windows don't receive onWheelChange instead
		if (wheelTarget)
		{
			wheelTarget->activate();
			wheelTarget->captureInput();
		}

		// inject
		wheelInjected = mCegui->injectMouseWheelChange(float(mi.wheelDelta) / 75.0f);

		// restore previously active window
		if (wheelTarget)
		{
			wheelTarget->releaseInput();
			if (prevActive) prevActive->activate();
		}
	}

	bool positionInjected = mCegui->injectMousePosition(float(mi.x), float(mi.y));

	return wheelInjected || positionInjected;
}

bool GUIMgr::MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
{
	OC_DASSERT(mCegui);
	mCurrentInputEvent.mouseInfo = &mi;
	mCurrentInputEvent.mouseButton = btn;
	bool result = mCegui->injectMouseButtonDown(ConvertMouseButtonEnum(btn));
	return result;
}

bool GUIMgr::MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
{
	OC_DASSERT(mCegui);
	mCurrentInputEvent.mouseInfo = &mi;
	mCurrentInputEvent.mouseButton = btn;
	gEditorMgr.EnablePopupClosing();
	bool result = mCegui->injectMouseButtonUp(ConvertMouseButtonEnum(btn));
	gEditorMgr.CloseAllPopupMenus();
	return result;
}

void GUIMgr::ResolutionChanging(const uint32 width, const uint32 height)
{
	OC_UNUSED(width);
	OC_UNUSED(height);
	OC_DASSERT(mCegui);

	mRenderer->grabTextures();

	return;
}

void GUIMgr::ResolutionChanged(const uint32 width, const uint32 height)
{
	OC_UNUSED(width);
	OC_UNUSED(height);
	OC_DASSERT(mCegui);

	mRenderer->restoreTextures();

	return mCegui->notifyDisplaySizeChanged(CEGUI::Size((float32)width, (float32)height));
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

bool LayoutPropertyCallback(StringSystem::StringMgr& stringMgr, CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
{
	OC_UNUSED(userdata);
	if ((propname == "Text" || propname == "Tooltip") &&
		propvalue.size() > 2 &&
		propvalue.at(0) == '$' &&
		propvalue.at(propvalue.size() - 1) == '$')
	{
		/// Use StringMgr to translate textual data in GUI.
		CEGUI::String translatedText = stringMgr.GetTextData(GUIMgr::GUIGroup, propvalue.substr(1, propvalue.size() - 2).c_str());
		window->setProperty(propname, translatedText);
		return false;
	}
	return true;
}

bool SystemLayoutPropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
{
	return LayoutPropertyCallback(gStringMgrSystem, window, propname, propvalue, userdata);
}

bool ProjectLayoutPropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
{
	return LayoutPropertyCallback(gStringMgrProject, window, propname, propvalue, userdata);
}


