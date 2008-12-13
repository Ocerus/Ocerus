#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "../InputSystem/IInputListener.h"
#include "../Utility/Settings.h"
#include "CEGUI.h"
#include <set>

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

namespace GUISystem {

	/// Forward declarations.
	//@{
	class ResourceGate;
	class RendererGate;
	//@}

	//TODO vsechny inlinovany funkce premistit do CPP a odinlinovat, at se zbavime includovani CEGUI.h

	class IConsoleListener {
	public:
		virtual void EventConsoleCommand(std::string command) = 0;
	};

	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener
	{
	public:	
		GUIMgr();

		/// DO NOT CHANGE TO const METHODS!! these methods override IInputListener's methods
		//@{
		virtual void KeyPressed(const InputSystem::KeyInfo& ke);

		virtual void KeyReleased(const InputSystem::KeyInfo& ke);

		virtual void MouseMoved(const InputSystem::MouseInfo& mi);

		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		//@}

		virtual void LoadGUI();
		virtual void LoadConsole();

		inline virtual void RenderGUI() const {
			assert(mCegui);
			CEGUI::System::getSingleton().renderGUI();
		}

		virtual void Update(float32 delta);

		void AddConsoleListener(IConsoleListener* listener);
		void AddConsoleMessage(std::string message, const GfxSystem::Color& color = GfxSystem::Color(255,255,255,255));

		virtual ~GUIMgr();
	protected:
		void RegisterEvents();
		bool QuitEvent(const CEGUI::EventArgs& e);
		void ConsoleTrigger();
		bool ConsoleCommandEvent(const CEGUI::EventArgs& e);
		void EnsureConsoleIsLoaded();

		bool ConsoleIsLoaded;
		CEGUI::System * mCegui;
		CEGUI::Window * CurrentWindowRoot;
		ResourceGate * mResourceGate;
		RendererGate * mRendererGate;
		friend class RendererGate;

		std::set<IConsoleListener*> ConsoleListeners;
	};
}
#endif
