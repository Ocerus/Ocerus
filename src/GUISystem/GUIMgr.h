#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "InputSystem/IInputListener.h"
#include "GfxSystem/IGfxWindowListener.h"

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
	class ResourceProvider;
	class GUIConsole;

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
		void LoadRootLayout(const string& filename);

		/// Unloads loaded root layout or does nothing, if no root layout is loaded.
		void UnloadRootLayout();

		/// Returns current root layout.
		CEGUI::Window* GetRootLayout() const { return mCurrentRootLayout; }

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
		virtual void KeyPressed(const InputSystem::KeyInfo& ke);
		virtual void KeyReleased(const InputSystem::KeyInfo& ke);
		virtual void MouseMoved(const InputSystem::MouseInfo& mi);
		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		//@}

		/// This method injects resolution change into GUI system. It is part of IGfxWindowListener interface.
		virtual void ResolutionChanged(int32 width, int32 height);

		inline GUIConsole* GetConsole();

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
