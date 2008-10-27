#include "OISListener.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../Utility/StringConverter.h"
#include "IInputListener.h"
#include <OISInputManager.h>

using namespace InputSystem;


InputSystem::OISListener::OISListener()
{
	mMgr = InputMgr::GetSingletonPtr();

	OIS::ParamList pl;
	uint64 hWnd = GfxSystem::GfxRenderer::GetSingleton()._GetWindowHandle();
	pl.insert(OIS::ParamList::value_type("WINDOW", /*StringConverter::NumToStr(hWnd)*/"0"));
	mOIS = OIS::InputManager::createInputSystem(pl);
	mMouse = static_cast<OIS::Mouse*>(mOIS->createInputObject(OIS::OISMouse, true));
	mKeyboard = static_cast<OIS::Keyboard*>(mOIS->createInputObject(OIS::OISKeyboard, true));
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);
}

InputSystem::OISListener::~OISListener()
{
	assert(mOIS);
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
	eMouseButton btn = OisToMbtn(id);
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
		(*i)->MouseButtonPressed(btn);
	return true;
}

bool InputSystem::OISListener::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
	eMouseButton btn = OisToMbtn(id);
	for (InputMgr::ListenersList::const_iterator i=mMgr->mListeners.begin(); i!=mMgr->mListeners.end(); ++i)
		(*i)->MouseButtonReleased(btn);
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
	assert(mMouse && mKeyboard);
	mMouse->capture();
	mKeyboard->capture();
}

void InputSystem::OISListener::SetResolution( uint32 width, uint32 height )
{
	assert(mMouse);
	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
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
	}
	return MBTN_UNKNOWN;
}

bool InputSystem::OISListener::IsKeyDown( eKeyCode k )
{
	assert(mKeyboard);
	return mKeyboard->isKeyDown(static_cast<OIS::KeyCode>(k));
}