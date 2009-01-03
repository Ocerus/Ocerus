#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "../InputSystem/IInputListener.h"
#include "../Utility/Settings.h"
#include "StaticElements.h"
#include "CEGUIBase.h"
#include "CEGUISystem.h"
#include "CEGUIWindow.h"
#include <set>

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

namespace GUISystem {

	/// @name Forward declarations.
	//@{
	class ResourceGate;
	class RendererGate;
	//@}

	//TODO vsechny inlinovany funkce premistit do CPP a odinlinovat, at se zbavime includovani CEGUI.h

	// Inherit this to be eligible to console prompt events
	class IConsoleListener {
	public:
		virtual void EventConsoleCommand(string command) = 0;
	};

	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener
	{
	public:	
		GUIMgr();

		/** @name IInputListener implementation
		  * DO NOT CHANGE TO const METHODS!!
	      */
		//@{
		virtual void KeyPressed(const InputSystem::KeyInfo& ke);

		virtual void KeyReleased(const InputSystem::KeyInfo& ke);

		virtual void MouseMoved(const InputSystem::MouseInfo& mi);

		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		//@}

		/// @name Don't call this now
		virtual void LoadGUI();

		/// @name Loads basic stuff common for all schemes.
		void LoadStyle(void);

		/// @name Called in main application loop
		//@{
		inline virtual void RenderGUI() const {
			assert(mCegui);
			CEGUI::System::getSingleton().renderGUI();
		}
		
		virtual void Update(float32 delta);
		//@}

		/// @name Console related methods
		//@{
		/// @name Registers a class that implements IConsoleListener
		void AddConsoleListener(IConsoleListener* listener);
		/// @name If you wish to post a new message into console, call this method
		void AddConsoleMessage(string message, const GfxSystem::Color& color = GfxSystem::Color(255,255,255,255));
		bool IsConsoleLoaded(void) const { return mConsoleIsLoaded; }
		//@}

		/// @name Static text related methods
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
		/// @name Registers callbacks in CEGUI
		void RegisterEvents();

		/// @name CEGUI events callbacks
		//@{
		bool QuitEvent(const CEGUI::EventArgs& e);
		bool ConsoleCommandEvent(const CEGUI::EventArgs& e);
		//@}

		/// @name Called after ` is hit
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
		std::deque<string>::const_iterator mCurrentLastSelected;
		std::deque<string> mLastCommands;
		//@}

		std::set<IConsoleListener*> mConsoleListeners;		
		std::map<string, StaticElement*> mCreatedStaticElements;
	};
}
#endif
