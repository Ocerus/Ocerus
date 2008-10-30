#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "../InputSystem/IInputListener.h"
#include "../InputSystem/InputActions.h"
#include "CEGUI.h"

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

//#pragma once

namespace GUISystem {

	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener
	{
	public:	
		GUIMgr(void);

		virtual void KeyPressed(const InputSystem::KeyInfo& ke);
		virtual void KeyReleased(const InputSystem::KeyInfo& ke);

		virtual void MouseMoved(const InputSystem::MouseInfo& mi);
		virtual void MouseButtonPressed(const InputSystem::eMouseButton btn);
		virtual void MouseButtonReleased(const InputSystem::eMouseButton btn);

		virtual ~GUIMgr();
	protected:
		CEGUI::System * mCegui;
	};
}
#endif
