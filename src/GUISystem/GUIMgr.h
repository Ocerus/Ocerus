#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "InputSystem/IInputListener.h"
#include "GfxSystem/IGfxWindowListener.h"
#include <CEGUIEvent.h>

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

namespace CEGUI
{
	class Listbox;
	class OpenGLRenderer;
	class System;
	class Window;
	class EventArgs;
	class String;
}

namespace GUISystem
{
	/**
	 * The IConsoleListener interface defines an interface for listening to
	 * commands from in-game console.
	 */
	class IConsoleListener {
	public:
		virtual void EventConsoleCommand(string command) = 0;
	};

	/**
	 * The GUIMgr class manages the GUI.
	 * ...
	 * It MUST be created after InputMgr, because it registers for user input.
	 */
	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener, public GfxSystem::IGfxWindowListener
	{
	public:
		/// Constructs a GUIMgr. Do not use this directly, use CreateSingleton() instead.
		/// Also note that InputMgr must be initialized before construction of GUIMgr.
		GUIMgr();

		/// Destroys the GUIMgr.
		virtual ~GUIMgr();

		/// Initializes GUI.
		void Init();

		/// Deinitializes GUI.
		void Deinit();

		/// Loads GUI layout from filename and sets it as root layout of the window.
		/// Automatically unloads previously loaded root layout.
		bool LoadRootLayout(const string& filename);

		/// Unloads loaded root layout or does nothing, if no root layout is loaded.
		void UnloadRootLayout();

		/// Returns current root layout.
		CEGUI::Window* GetRootLayout() const { return mCurrentRootLayout; }

		/// Disconnects the event from its handler.
		void DisconnectEvent(const CEGUI::Event::Connection eventConnection);

		/// Makes sure to actually disconnect the events.
		void ProcessDisconnectedEventList();

		/// Renders the GUI.
		void RenderGUI() const;

		/// Injects given amount of time into GUI system.
		virtual void Update(float32 delta);

		/// Loads the GUI layout from file and returns pointer to Window.
		/// @see CEGUI::WindowManager::loadWindowLayout
		CEGUI::Window* LoadWindowLayout(const string& filename, const string& name_prefix = "", const string& resourceGroup = "");

		/// @name IInputListener interface methods
		/// Those methods inject input into GUI system.
		//@{
		virtual bool KeyPressed(const InputSystem::KeyInfo& ke);
		virtual bool KeyReleased(const InputSystem::KeyInfo& ke);
		virtual bool MouseMoved(const InputSystem::MouseInfo& mi);
		virtual bool MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		virtual bool MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		//@}

		/// This method injects resolution change into GUI system. It is part of IGfxWindowListener interface.
		virtual void ResolutionChanged(const uint32 width, const uint32 height);

		inline GUIConsole* GetConsole();
		
		static const StringKey GUIGroup;

#if 0   //TODO
		/// Static text related methods
		//@{
		void AddStaticText( float32 x, float32 y, const string & id, const string & text,
			const GfxSystem::Color color/*TODO:Gfx = GfxSystem::Color(255,255,255)*/,
			uint8 text_anchor = ANCHOR_LEFT | ANCHOR_TOP,
			uint8 screen_anchor = ANCHOR_LEFT | ANCHOR_TOP,
			const string & fontid = "");
		Vector2 GetTextSize( const string & text, const string & fontid = "" );
		StaticText* GetStaticText( const string & id );
		//@}
#endif

	private:
		CEGUI::System* mCegui;
		CEGUI::Window* mWindowRoot;
		CEGUI::Window* mCurrentRootLayout;

		list<CEGUI::Event::Connection> mDeadEventConnections;

		GUIConsole* mGUIConsole;

		CEGUI::OpenGLRenderer* mRenderer;
		ResourceProvider* mResourceProvider;


#if 0   //TODO
		/// @name Commands memory
		//@{
		void AddLastCommand(string command);
		void LoadLastCommand();
		deque<string>::const_iterator mCurrentLastSelected;
		deque<string> mLastCommands;
		//@}
#endif

		/// Collection of ConsoleListeners.
		typedef vector<IConsoleListener*> ConsoleListeners;
		ConsoleListeners mConsoleListeners;

		bool mConsoleIsLoaded;


		//map<string, StaticElement*> mCreatedStaticElements;
	};

	inline GUIConsole* GUIMgr::GetConsole()
	{
		return mGUIConsole;
	}
}
#endif
