#include "Common.h"
#include "GUIMgr.h"

#include "CEGUICommon.h"
#include "GUIConsole.h"
#include "PopupMgr.h"
#include "ResourceProvider.h"
#include "ScriptProvider.h"
#include "ViewportWindow.h"

#include "Core/Application.h"
#include "InputSystem/InputMgr.h"
#include "InputSystem/InputActions.h"
#include "Editor/EditorMgr.h"

#include <RendererModules/OpenGL/CEGUIOpenGLRenderer.h>

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
	mCEGUI(0),
	mRenderer(0),
	mResourceProvider(0),
	mScriptProvider(0),
	mGUIConsole(0)
{
}

GUIMgr::~GUIMgr()
{
	delete mGUIConsole;
	mGUIConsole = 0;
	delete mPopupMgr;
	mPopupMgr = 0;

	gGfxWindow.RemoveScreenListener(this);
	gInputMgr.RemoveInputListener(this);

	ocInfo << "Destroying CEGUI";
	CEGUI::System::destroy();
	CEGUI::OpenGLRenderer::destroy(*mRenderer);
	ocInfo << "CEGUI destroyed";

	delete mResourceProvider;
	delete mScriptProvider;
}

void GUIMgr::Init(const string& ceguiLogPath)
{
	ocInfo << "*** GUIMgr init ***";

	mRenderer = &CEGUI::OpenGLRenderer::create();
	mResourceProvider = new ResourceProvider();
	mScriptProvider = new ScriptProvider();
	mCEGUI = &CEGUI::System::create(*mRenderer, mResourceProvider, 0, 0, mScriptProvider, "", ceguiLogPath);

	gInputMgr.AddInputListener(this);
	gGfxWindow.AddScreenListener(this);

	CEGUI::Imageset::setDefaultResourceGroup("gui-imagesets");
	CEGUI::Font::setDefaultResourceGroup("gui-fonts");
	CEGUI::Scheme::setDefaultResourceGroup("gui-schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("gui-looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("gui-layouts");
	mGUIConsole = new GUIConsole();
	mPopupMgr = new PopupMgr();
}

void GUIMgr::InitResources()
{
	LoadGeneralResources();

	if (gApp.IsDevelopMode())
	{
		LoadEditorResources();
	}
}

void GUIMgr::LoadGeneralResources()
{
	/// Register CEGUI system resources.
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "general/gui/schemes", "gui-schemes", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "general/gui/imagesets", "gui-imagesets", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "general/gui/fonts", "gui-fonts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "general/gui/looknfeel", "gui-looknfeels", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);

	OC_CEGUI_TRY;
	{
		CEGUI::SchemeManager::getSingleton().create("general/gui/schemes/TaharezLook.scheme");

		// Set defaults
		mCEGUI->setDefaultFont("DejaVuSans-10");
		mCEGUI->setDefaultMouseCursor("TaharezLook", "MouseArrow");
		mCEGUI->setMouseClickEventGenerationEnabled(true);
	}
	OC_CEGUI_CATCH_CRITICAL;
}

void GUIMgr::LoadEditorResources()
{
	/// Register CEGUI system resources.
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/schemes", "gui-schemes", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/imagesets", "gui-imagesets", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/fonts", "gui-fonts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/layouts", "gui-layouts", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "gui/looknfeel", "gui-looknfeels", ".*", "", ResourceSystem::RESTYPE_CEGUIRESOURCE);

	OC_CEGUI_TRY;
	{
		// Register custom Window subclasses to the CEGUI
		CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<ViewportWindow> >();

		CEGUI::SchemeManager::getSingleton().create("gui/schemes/VanillaSkin.scheme");
		CEGUI::SchemeManager::getSingleton().create("gui/schemes/Editor.scheme");

		// Set defaults
		mCEGUI->setDefaultFont("DejaVuSans-10");
		mCEGUI->setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
		mCEGUI->setMouseClickEventGenerationEnabled(true);
	}
	OC_CEGUI_CATCH_CRITICAL;
}

void GUIMgr::Deinit()
{
	DeinitConsole();
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
	OC_ASSERT(mCEGUI);
	OC_CEGUI_TRY;
	{
		CEGUI::SchemeManager::getSingleton().create(filename);
	}
	OC_CEGUI_CATCH;
}

string GUIMgr::LoadProjectScheme(const string& filename)
{
	OC_ASSERT(mCEGUI);
	OC_CEGUI_TRY;
	{
		CEGUI::String oldResourceGroup = mCEGUI->getResourceProvider()->getDefaultResourceGroup();
		CEGUI::String oldImagesetGroup = CEGUI::Imageset::getDefaultResourceGroup();
		CEGUI::String oldFontGroup = CEGUI::Font::getDefaultResourceGroup();
		mCEGUI->getResourceProvider()->setDefaultResourceGroup("Project");
		CEGUI::Imageset::setDefaultResourceGroup("Project");
		CEGUI::Font::setDefaultResourceGroup("Project");
		CEGUI::Scheme& scheme = CEGUI::SchemeManager::getSingleton().create(filename, "Project");
		mCEGUI->getResourceProvider()->setDefaultResourceGroup(oldResourceGroup);
		CEGUI::Imageset::setDefaultResourceGroup(oldImagesetGroup);
		CEGUI::Font::setDefaultResourceGroup(oldFontGroup);
		return scheme.getName().c_str();
	}
	OC_CEGUI_CATCH;
	return "unknown";
}

void GUISystem::GUIMgr::UnloadProjectScheme(const string& schemeName)
{
	OC_ASSERT(mCEGUI);
	CEGUI::SchemeManager::getSingleton().destroy(schemeName);
}

void GUIMgr::LoadSystemImageset(const string& filename)
{
	OC_DASSERT(mCEGUI);
	OC_CEGUI_TRY;
	{
		CEGUI::ImagesetManager::getSingleton().create("gui/imagesets/" + filename);
	}
	OC_CEGUI_CATCH;

}

CEGUI::Window* GUIMgr::LoadSystemLayout(const CEGUI::String& filename, const CEGUI::String& namePrefix)
{
	OC_DASSERT(mCEGUI);
	CEGUI::Window* resultLayout = 0;

	OC_CEGUI_TRY;
	{
		resultLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout("gui/layouts/" + filename, namePrefix, "", SystemLayoutPropertyCallback);
	}
	OC_CEGUI_CATCH;

	if (resultLayout) ocInfo << "System GUI layout " << filename << " loaded.";
	else ocWarning << "Cannot load system GUI layout " << filename << ".";

	return resultLayout;
}

CEGUI::Window* GUIMgr::LoadProjectLayout(const CEGUI::String& filename, const CEGUI::String& namePrefix)
{
	OC_DASSERT(mCEGUI);
	CEGUI::Window* resultLayout = 0;

	OC_CEGUI_TRY;
	{
		resultLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, namePrefix, "Project", ProjectLayoutPropertyCallback);
	}
	OC_CEGUI_CATCH;

	if (resultLayout) ocInfo << "Project GUI layout " << filename << " loaded.";
	else ocWarning << "Cannot load project GUI layout " << filename << ".";

	return resultLayout;
}

bool GUIMgr::SetGUISheet(CEGUI::Window* sheet)
{
	OC_DASSERT(mCEGUI);
	if (sheet)
	{
		mCEGUI->setGUISheet(sheet);
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
	OC_DASSERT(mCEGUI);
	return mCEGUI->getGUISheet();
}

CEGUI::Window* GUISystem::GUIMgr::GetWindow(const CEGUI::String& name)
{
	return CEGUI::WindowManager::getSingleton().getWindow(name.c_str());
}

bool GUISystem::GUIMgr::WindowExists(const string& name)
{
	return CEGUI::WindowManager::getSingleton().isWindowPresent(name.c_str());
}

#undef CreateWindow

CEGUI::Window* GUIMgr::CreateWindow(const CEGUI::String& type, const CEGUI::String& name)
{
	return CEGUI::WindowManager::getSingleton().createWindow(type, name);
}

void GUISystem::GUIMgr::DestroyWindow(CEGUI::Window* window)
{
	CEGUI::WindowManager::getSingleton().destroyWindow(window);
}

void GUIMgr::RenderGUI() const
{
	OC_DASSERT(mCEGUI);
	mCEGUI->renderGUI();
}

void GUIMgr::Update(float32 delta)
{
	OC_DASSERT(mCEGUI);
	mCEGUI->injectTimePulse(delta);
}

bool GUIMgr::KeyPressed(const InputSystem::KeyInfo& ke)
{
	OC_DASSERT(mCEGUI);
	mCurrentInputEvent.keyInfo = &ke;

	// ToggleConsole button is hardwired here.
	if (ke.keyCode == InputSystem::KC_GRAVE) {
		mGUIConsole->ToggleConsole();
		return true;
	}

	if (!mCEGUI->injectKeyDown(KeyMapperOIStoCEGUI(ke.keyCode)))
	{
		return mCEGUI->injectChar(ke.charCode);
	}

	return false;
}

bool GUIMgr::KeyReleased(const InputSystem::KeyInfo& ke)
{
	OC_DASSERT(mCEGUI);
	mCurrentInputEvent.keyInfo = &ke;
	return mCEGUI->injectKeyUp(KeyMapperOIStoCEGUI(ke.keyCode));
}

bool GUIMgr::MouseMoved(const InputSystem::MouseInfo& mi)
{
	OC_DASSERT(mCEGUI);
	mCurrentInputEvent.mouseInfo = &mi;

	bool wheelInjected = false;
	if (mi.wheelDelta != 0)
	{
		CEGUI::Window* wheelTarget = CEGUI::System::getSingleton().getWindowContainingMouse();

		// save the currently active window
		CEGUI::Window* root = CEGUI::System::getSingleton().getGUISheet();
		CEGUI::Window* prevActive = root->getActiveChild();

		// find the first window wanting the mouse wheel
		while (wheelTarget)
		{
			if (wheelTarget->isVisible() && !wheelTarget->isDisabled() &&
				wheelTarget->isUserStringDefined("WantsMouseWheel") &&
				wheelTarget->getUserString("WantsMouseWheel") == "True")
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
		wheelInjected = mCEGUI->injectMouseWheelChange(float(mi.wheelDelta) / 75.0f);

		// restore previously active window
		if (wheelTarget)
		{
			wheelTarget->releaseInput();
			if (prevActive) prevActive->activate();
		}
	}

	bool positionInjected = mCEGUI->injectMousePosition(float(mi.x), float(mi.y));

	return wheelInjected || positionInjected;
}

bool GUIMgr::MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
{
	OC_DASSERT(mCEGUI);
	mCurrentInputEvent.mouseInfo = &mi;
	mCurrentInputEvent.mouseButton = btn;
	bool result = mCEGUI->injectMouseButtonDown(ConvertMouseButtonEnum(btn));
	return result;
}

bool GUIMgr::MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn)
{
	OC_DASSERT(mCEGUI);
	mCurrentInputEvent.mouseInfo = &mi;
	mCurrentInputEvent.mouseButton = btn;
	GetPopupMgr()->DoAutoHide((float)mi.x, (float)mi.y);
	bool result = mCEGUI->injectMouseButtonUp(ConvertMouseButtonEnum(btn));
	return result;
}

void GUIMgr::ResolutionChanging(const uint32 width, const uint32 height)
{
	OC_UNUSED(width);
	OC_UNUSED(height);
	OC_DASSERT(mCEGUI);

	mRenderer->grabTextures();

	return;
}

void GUIMgr::ResolutionChanged(const uint32 width, const uint32 height)
{
	OC_UNUSED(width);
	OC_UNUSED(height);
	OC_DASSERT(mCEGUI);

	mRenderer->restoreTextures();

	return mCEGUI->notifyDisplaySizeChanged(CEGUI::Size((float32)width, (float32)height));
}

bool GUIMgr::Update()
{ 
	bool result = mNeedsUpdate;
	mNeedsUpdate = false;
	return result;
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
		// Using StringMgr to translate textual data in GUI.
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


