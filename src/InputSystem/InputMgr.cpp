#include "Common.h"
#include "InputMgr.h"
#include "IInputListener.h"
#include "OISListener.h"
#include "../GfxSystem/GfxRenderer.h"
#include <algorithm>

using namespace InputSystem;

InputSystem::InputMgr::InputMgr( void )
{
	gLogMgr.LogMessage("*** InputMgr init ***");
	mOISListener = DYN_NEW OISListener();

	gGfxRenderer.AddScreenListener(this);
	ResolutionChanged(gGfxRenderer.GetResolution().x, gGfxRenderer.GetResolution().y);
}

InputSystem::InputMgr::~InputMgr( void )
{
	ASSERT(mOISListener);
	DYN_DELETE mOISListener;
}

void InputSystem::InputMgr::CaptureInput( void )
{
	ASSERT(mOISListener);
	mOISListener->CaptureInput();
}

void InputSystem::InputMgr::AddInputListener( IInputListener* listener )
{
	ASSERT(listener);
	mListeners.push_back(listener);
}

void InputSystem::InputMgr::RemoveInputListener( IInputListener* listener )
{
	ASSERT(listener);
	ListenersList::iterator it = std::find(mListeners.begin(), mListeners.end(), listener);
	if (it != mListeners.end())
		mListeners.erase(it);
}

void InputSystem::InputMgr::RemoveAllInputListeners( void )
{
	mListeners.clear();
}

void InputSystem::InputMgr::ResolutionChanged( int width, int height )
{
	ASSERT(mOISListener);
	mOISListener->SetResolution(width, height);
}

bool InputSystem::InputMgr::IsKeyDown( const eKeyCode k ) const
{
	ASSERT(mOISListener);
	return mOISListener->IsKeyDown(k);
}

MouseState& InputSystem::InputMgr::GetMouseState( void ) const
{
	mOISListener->GetMouseState(mMouseStateCache);
	return mMouseStateCache;
}

bool InputSystem::InputMgr::IsMouseButtonPressed( const eMouseButton btn ) const
{
	MouseState& mouse = GetMouseState();
	return (mouse.buttons & btn) != 0;
}

