#include "InputMgr.h"
#include "IInputListener.h"
#include "OISListener.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../Common.h"
#include <algorithm>

using namespace InputSystem;


InputSystem::InputMgr::InputMgr( void )
{
	gLogMgr.LogMessage("*** InputMgr init ***");
	mOISListener = DYN_NEW OISListener();
	_SetResolution(gGfxRenderer.GetResolution().x, gGfxRenderer.GetResolution().y);
}

InputSystem::InputMgr::~InputMgr( void )
{
	assert(mOISListener);
	DYN_DELETE mOISListener;
}

void InputSystem::InputMgr::CaptureInput( void )
{
	assert(mOISListener);
	mOISListener->CaptureInput();
}

void InputSystem::InputMgr::AddInputListener( IInputListener* listener )
{
	assert(listener);
	mListeners.push_back(listener);
}

void InputSystem::InputMgr::RemoveInputListener( IInputListener* listener )
{
	assert(listener);
	ListenersList::const_iterator it = std::find(mListeners.begin(), mListeners.end(), listener);
	if (it != mListeners.end())
		mListeners.erase(it);
}

void InputSystem::InputMgr::RemoveAllInputListeners( void )
{
	mListeners.clear();
}

void InputSystem::InputMgr::_SetResolution( uint32 width, uint32 height )
{
	assert(mOISListener);
	mOISListener->SetResolution(width, height);
}

bool InputSystem::InputMgr::IsKeyDown( eKeyCode k )
{
	assert(mOISListener);
	return mOISListener->IsKeyDown(k);
}