#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "../InputSystem/InputMgr.h"
#include "../InputSystem/IInputListener.h"
#include "../InputSystem/InputActions.h"
#include "../Utility/Settings.h"
#include "RendererGate.h"
#include "ResourceGate.h"
#include "CEGUI.h"

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

//#pragma once

namespace GUISystem {
	class ResourceGate;
	class RendererGate;

	inline CEGUI::MouseButton ConvertMouseButtonEnum(const InputSystem::eMouseButton btn) {
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

	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener
	{
	public:	
		GUIMgr(void);

		inline virtual void KeyPressed(const InputSystem::KeyInfo& ke) {
			CEGUI::System::getSingleton().injectKeyDown(ke.keyCode);
		}

		inline virtual void KeyReleased(const InputSystem::KeyInfo& ke) {
			CEGUI::System::getSingleton().injectKeyUp(ke.keyCode);
		}

		inline virtual void MouseMoved(const InputSystem::MouseInfo& mi) {
			CEGUI::System::getSingleton().injectMousePosition(float(mi.x), float(mi.y));
			CEGUI::System::getSingleton().injectMouseWheelChange(float(mi.wheelDelta));
		}

		inline virtual void MouseButtonPressed(const InputSystem::eMouseButton btn) {
			CEGUI::System::getSingleton().injectMouseButtonDown( ConvertMouseButtonEnum(btn) );
		}

		inline virtual void MouseButtonReleased(const InputSystem::eMouseButton btn) {
			CEGUI::System::getSingleton().injectMouseButtonUp( ConvertMouseButtonEnum(btn) );
		}

		inline virtual void RenderGUI() { mCegui->renderGUI(); }

		virtual void Update(float32 delta);

		virtual ~GUIMgr();
	protected:
		CEGUI::System * mCegui;
		ResourceGate * mResourceGate;
		RendererGate * mRendererGate;
		friend class RendererGate;
	};
}
#endif
