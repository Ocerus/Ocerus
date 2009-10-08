#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "../InputSystem/IInputListener.h"
#include "StaticElements.h"
#include "CEGUIBase.h"
#include "CEGUISystem.h"
#include "CEGUIWindow.h"

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

namespace GUISystem {

	class ResourceGate;
	class RendererGate;

	class IConsoleListener {
	public:
		virtual void EventConsoleCommand(string command) = 0;
	};

	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener
	{
	public:	

		GUIMgr();

		virtual void KeyPressed(const InputSystem::KeyInfo& ke);

		virtual void KeyReleased(const InputSystem::KeyInfo& ke);

		virtual void MouseMoved(const InputSystem::MouseInfo& mi);

		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		/// Don't call this now.
		virtual void LoadGUI();

		/// Loads basic stuff common for all schemes.
		void LoadStyle(void);

		inline virtual void RenderGUI() const {
			BS_ASSERT(mCegui);
			CEGUI::System::getSingleton().renderGUI();
		}
		
		virtual void Update(float32 delta);


		/// @name Console related methods
		//@{
		/// Registers a class that implements IConsoleListener/
		void AddConsoleListener(IConsoleListener* listener);
		/// If you wish to post a new message into console, call this method.
		void AddConsoleMessage(string message, const GfxSystem::Color& color = GfxSystem::Color(255,255,255,255));
		bool IsConsoleLoaded(void) const { return mConsoleIsLoaded; }
		//@}

		/// Static text related methods
		//@{
		void AddStaticText( float32 x, float32 y, const string & id, const string & text,
			const GfxSystem::Color color = GfxSystem::Color(255,255,255),
			uint8 text_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			uint8 screen_anchor = GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_TOP,
			const string & fontid = "");
		Vector2 GetTextSize( const string & text, const string & fontid = "" );
		StaticText* GetStaticText( const string & id );
		//@}

		virtual ~GUIMgr();

	protected:

		/// Registers callbacks in CEGUI.
		void RegisterEvents();

		/// @name CEGUI events callbacks
		//@{
		bool QuitEvent(const CEGUI::EventArgs& e);
		bool ConsoleCommandEvent(const CEGUI::EventArgs& e);
		//@}

		/// Called after ` is hit.
		void ConsoleTrigger();

		bool mConsoleIsLoaded;
		CEGUI::System * mCegui;
		CEGUI::Window * mCurrentWindowRoot;
		ResourceGate * mResourceGate;
		RendererGate * mRendererGate;
		friend class RendererGate;

		/// @name Commands memory
		//@{
		void AddLastCommand(string command);
		void LoadLastCommand();
		deque<string>::const_iterator mCurrentLastSelected;
		deque<string> mLastCommands;
		//@}

		set<IConsoleListener*> mConsoleListeners;		
		map<string, StaticElement*> mCreatedStaticElements;
	};
}
#endif
