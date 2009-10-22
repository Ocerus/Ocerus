#include "Common.h"
#include "InputMgr.h"
#include "OISListener.h"
#include "../GfxSystem/GfxRenderer.h"
#include "StringConverter.h"
#include "IInputListener.h"
#include <OISInputManager.h>

using namespace InputSystem;


InputSystem::OISListener::OISListener(): mOIS(0), mMouse(0), mKeyboard(0)
{
	gLogMgr.LogMessage("Initing OIS");

	mMgr = InputMgr::GetSingletonPtr();

	OIS::ParamList pl;

	// let the OIS know what window we have so that it can capture its events
	uint32 hWnd = GfxSystem::GfxRenderer::GetSingleton()._GetWindowHandle();
	pl.insert(OIS::ParamList::value_type("WINDOW", StringConverter::ToString(hWnd)));

	// let the standard mouse cursor be
	pl.insert(Containers::make_pair(string("w32_mouse"), string("DISCL_BACKGROUND" )));
	pl.insert(Containers::make_pair(string("w32_mouse"), string("DISCL_NONEXCLUSIVE")));

	mOIS = OIS::InputManager::createInputSystem(pl);
	gLogMgr.LogMessage("OIS created");
	mKeyboard = static_cast<OIS::Keyboard*>(mOIS->createInputObject(OIS::OISKeyboard, true));
	gLogMgr.LogMessage("OIS keyboard inited");
	mMouse = static_cast<OIS::Mouse*>(mOIS->createInputObject(OIS::OISMouse, true));
	gLogMgr.LogMessage("OIS mouse inited");
	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
}

InputSystem::OISListener::~OISListener()
{
	OC_ASSERT(mOIS);
	OIS::InputManager::destroyInputSystem(mOIS);
}

bool InputSystem::OISListener::mouseMoved( const OIS::MouseEvent &evt )
{
	MouseInfo mi;
	mi.x = evt.state.X.abs;
	mi.dx = evt.state.X.rel;
	mi.y = evt.state.Y.abs;
	mi.dy = evt.state.Y.rel;
	mi.wheel = evt.state.Z.abs;
	mi.wheelDelta = evt.state.Z.rel;
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
		(*i)->MouseMoved(mi);
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
		(*i)->MouseButtonPressed(mi, btn);
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
		(*i)->MouseButtonReleased(mi, btn);
	return true;
}

bool InputSystem::OISListener::keyPressed( const OIS::KeyEvent &evt )
{
	KeyInfo ki;
	ki.keyAction = static_cast<eKeyCode>(evt.key);
	ki.keyCode = evt.text;
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
		(*i)->KeyPressed(ki);
	return true;
}

bool InputSystem::OISListener::keyReleased( const OIS::KeyEvent &evt )
{
	KeyInfo ki;
	ki.keyAction = static_cast<eKeyCode>(evt.key);
	ki.keyCode = evt.text;
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
		(*i)->KeyReleased(ki);
	return true;
}

void InputSystem::OISListener::CaptureInput()
{
	if (mKeyboard)
		mKeyboard->capture();
	if (mMouse)
		mMouse->capture();
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
		return mKeyboard->isKeyDown(static_cast<OIS::KeyCode>(k));
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