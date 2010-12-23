#include "Common.h"
#include "InputMgr.h"
#include "OISListener.h"
#include "GfxSystem/GfxWindow.h"
#include "Core/Application.h"
#include "StringConverter.h"
#include "IInputListener.h"
#include <OISInputManager.h>
#include <OISException.h>

#ifdef __UNIX__
#include <X11/Xlib.h>
#endif

using namespace InputSystem;

#define OIS_EXCEPTION_BEGIN try {
	
#define OIS_EXCEPTION_END } catch (const OIS::Exception& e) { \
		ocError << "OIS Exception: " << e.eText; \
		CRITICAL_FAILURE("Last error was critical"); \
	}



InputSystem::OISListener::OISListener(): mOIS(0), mMouse(0), mKeyboard(0)
{
	OIS_EXCEPTION_BEGIN
	ocInfo << "Initing OIS";

	mMgr = InputMgr::GetSingletonPtr();

	OIS::ParamList pl;

    // let the OIS know what window we have so that it can capture its events
	GfxSystem::WindowHandle hWnd = GfxSystem::GfxWindow::GetSingleton()._GetWindowHandle();
	pl.insert(OIS::ParamList::value_type("WINDOW", StringConverter::ToString(hWnd)));

	// let the standard mouse cursor be
	pl.insert(OIS::ParamList::value_type("w32_mouse", "DISCL_BACKGROUND"));
	pl.insert(OIS::ParamList::value_type("w32_mouse", "DISCL_NONEXCLUSIVE"));
	pl.insert(OIS::ParamList::value_type("x11_mouse_grab", "false"));
	pl.insert(OIS::ParamList::value_type("x11_mouse_hide", "true"));
	pl.insert(OIS::ParamList::value_type("x11_keyboard_grab", "false"));
	pl.insert(OIS::ParamList::value_type("XAutoRepeatOn", "true"));

	mOIS = OIS::InputManager::createInputSystem(pl);
	ocInfo << "OIS created";

	RecreateDevices();
	OIS_EXCEPTION_END
}

InputSystem::OISListener::~OISListener()
{
	OC_ASSERT(mOIS);
	OIS::InputManager::destroyInputSystem(mOIS);
}

bool InputSystem::OISListener::mouseMoved( const OIS::MouseEvent &evt )
{
	if (evt.state.Z.rel != 0) {
		// the wheel has moved; check if the mouse is still above the window; if not, ignore the event
		if (!gApp.HasFocus() || evt.state.X.abs < 0 || evt.state.X.abs >= evt.state.width
			|| evt.state.Y.abs < 0 || evt.state.Y.abs >= evt.state.height)
		{
			return true;
		}
	}

	MouseInfo mi;
	mi.x = evt.state.X.abs;
	mi.dx = evt.state.X.rel;
	mi.y = evt.state.Y.abs;
	mi.dy = evt.state.Y.rel;
	mi.wheel = evt.state.Z.abs;
	mi.wheelDelta = evt.state.Z.rel;
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
	{
		if ((*i)->MouseMoved(mi)) break;
	}
	return true;
}

bool InputSystem::OISListener::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
	MouseInfo mi;
	mi.dx = 0;
	mi.dy = 0;
	mi.wheelDelta = 0;
	mi.x = evt.state.X.abs;
	mi.y = evt.state.Y.abs;
	mi.wheel = evt.state.Z.abs;
	eMouseButton btn = OisToMbtn(id);
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
	{
		if ((*i)->MouseButtonPressed(mi, btn)) break;
	}
	return true;
}

bool InputSystem::OISListener::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
	MouseInfo mi;
	mi.dx = 0;
	mi.dy = 0;
	mi.wheelDelta = 0;
	mi.x = evt.state.X.abs;
	mi.y = evt.state.Y.abs;
	mi.wheel = evt.state.Z.abs;
	eMouseButton btn = OisToMbtn(id);
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
	{
		//if ((*i)->MouseButtonReleased(mi, btn)) break;
		// mouse released must be propagated to all listeners
		((*i)->MouseButtonReleased(mi, btn));
	}
	return true;
}

bool InputSystem::OISListener::keyPressed( const OIS::KeyEvent &evt )
{
	KeyInfo ki;
	ki.keyCode = static_cast<eKeyCode>(evt.key);
	ki.charCode = evt.text;
	FixKeyInfo(ki);
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
	{
		if ((*i)->KeyPressed(ki)) break;
	}
	return true;
}

bool InputSystem::OISListener::keyReleased( const OIS::KeyEvent &evt )
{
	KeyInfo ki;
	ki.keyCode = static_cast<eKeyCode>(evt.key);
	ki.charCode = evt.text;
	FixKeyInfo(ki);
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
	{
		if ((*i)->KeyReleased(ki)) break;
	}
	return true;
}

void InputSystem::OISListener::CaptureInput()
{
	if (mKeyboard)
	{
		mKeyboard->capture();
	}
	if (mMouse)
	{
		mMouse->capture();
	}
}

void InputSystem::OISListener::SetResolution( uint32 width, uint32 height )
{
	if (mMouse)
	{
		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;
	}
}

InputSystem::eMouseButton InputSystem::OISListener::OisToMbtn( OIS::MouseButtonID id )
{
	switch (id)
	{
	case OIS::MB_Left:
		return MBTN_LEFT;
	case OIS::MB_Right:
		return MBTN_RIGHT;
	case OIS::MB_Middle:
		return MBTN_MIDDLE;
	default:
		break;
	}
	return MBTN_UNKNOWN;
}

bool InputSystem::OISListener::IsKeyDown( const eKeyCode k ) const
{
	if (mKeyboard)
	{
		return mKeyboard->isKeyDown(static_cast<OIS::KeyCode>(k));
	}
	return false;
}

void InputSystem::OISListener::GetMouseState( InputSystem::MouseState& state ) const
{
	if (!mMouse)
		return;
	const OIS::MouseState& oisstate = mMouse->getMouseState();
	state.x = oisstate.X.abs;
	state.y = oisstate.Y.abs;
	state.wheel = oisstate.Z.abs;
	state.buttons = MBTN_UNKNOWN;
	if (oisstate.buttonDown(OIS::MB_Left))
		state.buttons |= MBTN_LEFT;
	if (oisstate.buttonDown(OIS::MB_Right))
		state.buttons |= MBTN_RIGHT;
	if (oisstate.buttonDown(OIS::MB_Middle))
		state.buttons |= MBTN_MIDDLE;
}

void InputSystem::OISListener::ReleaseAll( void )
{
	RecreateDevices();
}

void InputSystem::OISListener::RecreateDevices()
{
	OC_ASSERT(mOIS);
	
	int mouseWidth = 0;
	int mouseHeight = 0;

#ifdef __WIN__
	if (mKeyboard) mOIS->destroyInputObject(mKeyboard);
	if (mMouse)
	{
		const OIS::MouseState &ms = mMouse->getMouseState();
		mouseWidth = ms.width;
		mouseHeight = ms.height;
		mOIS->destroyInputObject(mMouse);
	}
	mKeyboard = 0;
	mMouse = 0;
#endif

	if (!mKeyboard)
	{
		OIS_EXCEPTION_BEGIN
		mKeyboard = static_cast<OIS::Keyboard*>(mOIS->createInputObject(OIS::OISKeyboard, true));
		mKeyboard->setEventCallback(this);
		OIS_EXCEPTION_END
	}

	if (!mMouse)
	{
		OIS_EXCEPTION_BEGIN
		mMouse = static_cast<OIS::Mouse*>(
#ifdef __WIN__	// We use modified OIS on Windows.
			mOIS->createInputObject(OIS::OISMouse, true, "", gGfxWindow.IsFullscreen())
#else
			mOIS->createInputObject(OIS::OISMouse, true, "")
#endif
		);
		mMouse->setEventCallback(this);
		OIS_EXCEPTION_END
	}

	if (mouseWidth != 0 && mouseHeight != 0)
	{
		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = mouseWidth;
		ms.height = mouseHeight;
	}

	ocInfo << "OIS keyboard & mouse recreated";
}

void InputSystem::OISListener::FixKeyInfo( KeyInfo& keyInfo )
{
	switch (keyInfo.keyCode)
	{
	case KC_NUMPAD0:
		keyInfo.charCode = '0';
		break;
	case KC_NUMPAD1:
		keyInfo.charCode = '1';
		break;
	case KC_NUMPAD2:
		keyInfo.charCode = '2';
		break;
	case KC_NUMPAD3:
		keyInfo.charCode = '3';
		break;
	case KC_NUMPAD4:
		keyInfo.charCode = '4';
		break;
	case KC_NUMPAD5:
		keyInfo.charCode = '5';
		break;
	case KC_NUMPAD6:
		keyInfo.charCode = '6';
		break;
	case KC_NUMPAD7:
		keyInfo.charCode = '7';
		break;
	case KC_NUMPAD8:
		keyInfo.charCode = '8';
		break;
	case KC_NUMPAD9:
		keyInfo.charCode = '9';
		break;
	case KC_DECIMAL:
		keyInfo.charCode = '.';
		break;
	case KC_DIVIDE:
		keyInfo.charCode = '/';
		break;
	default:
		break;
	}
}
