#include "Common.h"
#include "InputMgr.h"
#include "IInputListener.h"
#include "OISListener.h"
#include "../GfxSystem/GfxWindow.h"

using namespace InputSystem;

InputSystem::InputMgr::InputMgr( void )
{
	ocInfo << "*** InputMgr init ***";
	mOISListener = new OISListener();

	gGfxWindow.AddScreenListener(this);
	ResolutionChanged(gGfxWindow.GetCurrentResolutionWidth(), gGfxWindow.GetCurrentResolutionHeight());
}

InputSystem::InputMgr::~InputMgr( void )
{
	delete mOISListener;
}

void InputSystem::InputMgr::CaptureInput( void )
{
	PROFILE_FNC();
	OC_ASSERT(mOISListener);
	mOISListener->CaptureInput();
}

void InputSystem::InputMgr::AddInputListener( IInputListener* listener )
{
	OC_ASSERT(listener);
	mListeners.push_back(listener);
}

void InputSystem::InputMgr::AddInputListenerToFront( IInputListener* listener )
{
	OC_ASSERT(listener);
	mListeners.insert(mListeners.begin(), listener);
}

void InputSystem::InputMgr::RemoveInputListener( IInputListener* listener )
{
	OC_ASSERT(listener);
	ListenersList::iterator it = Containers::find(mListeners.begin(), mListeners.end(), listener);
	if (it != mListeners.end())
		mListeners.erase(it);
}

void InputSystem::InputMgr::RemoveAllInputListeners( void )
{
	mListeners.clear();
}

void InputSystem::InputMgr::ResolutionChanged( const uint32 width, const uint32 height )
{
	OC_ASSERT(mOISListener);
	mOISListener->SetResolution(width, height);
}

bool InputSystem::InputMgr::IsKeyDown( const eKeyCode k ) const
{
	OC_ASSERT(mOISListener);
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

void InputSystem::InputMgr::ReleaseAll( void )
{
	OC_ASSERT(mOISListener);
	mOISListener->ReleaseAll();
}