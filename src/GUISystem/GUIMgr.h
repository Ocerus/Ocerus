#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "../InputSystem/IInputListener.h"
#include "../Utility/Settings.h"
#include "CEGUI.h"

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

namespace GUISystem {

	/// Forward declarations.
	//@{
	class ResourceGate;
	class RendererGate;
	//@}

	//TODO vsechny inlinovany funkce premistit do CPP a odinlinovat, at se zbavime includovani CEGUI.h
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
		GUIMgr();

		/// DO NOT CHANGE TO const METHODS!! these methods override IInputListener's methods
		//@{
		inline virtual void KeyPressed(const InputSystem::KeyInfo& ke) {
			assert(mCegui);
			mCegui->injectKeyDown(ke.keyCode);
			mCegui->injectChar(ke.keyCode);
		}

		inline virtual void KeyReleased(const InputSystem::KeyInfo& ke) {
			assert(mCegui);
			mCegui->injectKeyUp(ke.keyCode);
		}

		inline virtual void MouseMoved(const InputSystem::MouseInfo& mi) {
			assert(mCegui);
			mCegui->injectMousePosition(float(mi.x), float(mi.y));
			mCegui->injectMouseWheelChange(float(mi.wheelDelta));
		}

		inline virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn) {
			assert(mCegui);
			mCegui->injectMouseButtonDown( ConvertMouseButtonEnum(btn) );
		}

		inline virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn) {
			assert(mCegui);
			mCegui->injectMouseButtonUp( ConvertMouseButtonEnum(btn) );
		}
		//@}

		virtual void LoadGUI();

		inline virtual void RenderGUI() const {
			assert(mCegui);
			mCegui->renderGUI();
		}

		virtual void Update(float32 delta);

		virtual ~GUIMgr();
	protected:
		void RegisterEvents();
		bool QuitEvent(const CEGUI::EventArgs& e);

		CEGUI::System * mCegui;
		CEGUI::Window * CurrentWindowRoot;
		ResourceGate * mResourceGate;
		RendererGate * mRendererGate;
		friend class RendererGate;
	};
}
#endif
