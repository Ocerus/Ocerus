#include "GUIMgr.h"
#include "CEGUI.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/IInputListener.h"
#include "../InputSystem/InputActions.h"

namespace GUISystem {
	GUIMgr::GUIMgr() {		
		gInputMgr.AddInputListener(this);
		// mCegui = new CEGUI::System( renderer )
	}

	GUIMgr::~GUIMgr() {
		gInputMgr.RemoveInputListener(this);
		// delete mCegui
	}

/*
// bool injectMouseMove( float delta_x, float delta_y );
+ bool injectMousePosition( float x_pos, float y_pos );
// bool injectMouseLeaves( void );
+ bool injectMouseButtonDown( MouseButton button );
+ bool injectMouseButtonUp( MouseButton button );
+ bool injectKeyDown( uint key_code );
+ bool injectKeyUp( uint key_code );
// bool injectChar( utf32 code_point );
+ bool injectMouseWheelChange( float delta );
- bool injectTimePulse( float timeElapsed ); - bude v jine casti
*/

	CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn) {
		switch (btn) {
			case InputSystem::MBTN_LEFT:
				return CEGUI::LeftButton;
			case InputSystem::MBTN_RIGHT:
				return CEGUI::RightButton;
			case InputSystem::MBTN_MIDDLE:
				return CEGUI::MiddleButton;
		}
		return CEGUI::NoButton;
	}

	void GUIMgr::KeyPressed(const InputSystem::KeyInfo& ke) {
		CEGUI::System::getSingleton().injectKeyDown(ke.keyCode);
	}

	void GUIMgr::KeyReleased(const InputSystem::KeyInfo& ke) {
		CEGUI::System::getSingleton().injectKeyUp(ke.keyCode);
	}

	void GUIMgr::MouseMoved(const InputSystem::MouseInfo& mi) {
		CEGUI::System::getSingleton().injectMousePosition(float(mi.x), float(mi.y));
		CEGUI::System::getSingleton().injectMouseWheelChange(float(mi.wheelDelta));
	}

	void GUIMgr::MouseButtonPressed(const InputSystem::eMouseButton btn) {
		CEGUI::System::getSingleton().injectMouseButtonDown( ConvertMouseButtonEnum(btn) );
	}

	void GUIMgr::MouseButtonReleased(const InputSystem::eMouseButton btn) {
		CEGUI::System::getSingleton().injectMouseButtonUp( ConvertMouseButtonEnum(btn) );
	}

}
