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
	/// The GUIMgr class manages the GUI. The manager depends on InputMgr.
	class GUIMgr : public Singleton<GUIMgr>, public InputSystem::IInputListener, public GfxSystem::IGfxWindowListener
	{
	public:

		/// Constructs a GUIMgr.
		GUIMgr();

		/// Destroys the GUIMgr.
		virtual ~GUIMgr();

		/// Initializes GUI.
		void Init();

		/// Deinitializes GUI.
		void Deinit();

		/// Initializes the in-game console. The GUI sheet must be set before calling InitConsole.
		void InitConsole();

		/// Deinitializes the in-game console.
		void DeinitConsole();

		/// Loads the system scheme file.
		void LoadSystemScheme(const string& filename);
		
		/// Loads the project scheme file.
		void LoadProjectScheme(const string& filename);
        
		/// Loads the system layout file and returns it.
		/// @param filename The layout filename.
		/// @param namePrefix Widget names in layout file are prefixed with given prefix.
		CEGUI::Window* LoadSystemLayout(const CEGUI::String& filename, const CEGUI::String& namePrefix = "");

		/// Loads the project layout file and returns it.
		/// @param filename The layout filename.
		/// @param namePrefix Widget names in layout file are prefixed with given prefix.
		CEGUI::Window* LoadProjectLayout(const CEGUI::String& filename, const CEGUI::String& namePrefix = "");

		/// Sets the active GUI sheet (root) window.
		bool SetGUISheet(CEGUI::Window* sheet);

		/// Returns a pointer to the active GUI sheet (root) window. Returns 0 if no GUI sheet is set.
		CEGUI::Window* GetGUISheet() const;

		/// Creates new window.
		///@param reallocateOnHeap If true the window is constructed anew before being used again.
		///	      This solves some problems with GUI elements aligning in a wrong way.
		///@param name Desired name. If empty a default name will be generated.
		CEGUI::Window* CreateWindow(const string& type, bool reallocateOnHeap = false, const string& name = "");

		/// Creates new window without taking caches into consideration.
		CEGUI::Window* CreateWindowDirectly(const string& type, const string& name = "");

		/// Destroys the specified window.
		void DestroyWindow(CEGUI::Window* window);

		/// Destroys the specified window without taking caches into consideration.
		void DestroyWindowDirectly(CEGUI::Window* window);

		/// Destroys all children of the given window.
		void DestroyWindowChildren(CEGUI::Window* window);

		/// Generates a new name for a window.
		string GenerateWindowName() const;

		/// Returns the window of the given name if exists.
		inline CEGUI::Window* GetWindow(const CEGUI::String& name) { return GetWindow(name.c_str()); }

		/// Returns the window of the given name if exists.
		inline CEGUI::Window* GetWindow(const char* name) { return GetWindow(string(name)); }

		/// Returns the window of the given name if exists.
		CEGUI::Window* GetWindow(const string& name);

		/// Returns true if the window of the given name exists.
		bool WindowExists(const string& name);
		
		/// Disconnects the event from its handler.
		/// @todo revision
		void DisconnectEvent(const CEGUI::Event::Connection eventConnection);

		/// Makes sure to actually disconnect the events.
		/// @todo revision
		void ProcessDisconnectedEventList();

		/// Renders the GUI.
		void RenderGUI() const;

		/// Injects given amount of time into GUI system.
		virtual void Update(float32 delta);

		/// @name IInputListener interface methods
		/// Those methods inject input into the GUI system.
		//@{
		virtual bool KeyPressed(const InputSystem::KeyInfo& ke);
		virtual bool KeyReleased(const InputSystem::KeyInfo& ke);
		virtual bool MouseMoved(const InputSystem::MouseInfo& mi);
		virtual bool MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		virtual bool MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		//@}

		struct InputEventInfo
		{
			const InputSystem::KeyInfo* keyInfo;
			const InputSystem::MouseInfo* mouseInfo;
			InputSystem::eMouseButton mouseButton;
		};

		/// Returns the info about the event being currently processed. This is valid only while any of the event handlers
		/// is still running.
		inline InputEventInfo& GetCurrentInputEvent() { return mCurrentInputEvent; }

		/// This method injects resolution change into GUI system. It is part of IGfxWindowListener interface. Called before window resolution is changed.
		virtual void ResolutionChanging(const uint32 width, const uint32 height);

		/// This method injects resolution change into GUI system. It is part of IGfxWindowListener interface. Called after window resolution is changed.
		virtual void ResolutionChanged(const uint32 width, const uint32 height);

		inline GUIConsole* GetConsole() { return mGUIConsole; }
		
		static const StringKey GUIGroup;


	private:

		CEGUI::System* mCegui;
		CEGUI::Window* mCurrentRootLayout;

		list<CEGUI::Event::Connection> mDeadEventConnections;
		InputEventInfo mCurrentInputEvent;

		GUIConsole* mGUIConsole;

		CEGUI::OpenGLRenderer* mRenderer;
		ResourceProvider* mResourceProvider;
		ScriptProvider* mScriptProvider;

		typedef vector<CEGUI::Window*> WindowList;
		struct WindowTypeCache { WindowList list; size_t count; };
		typedef hash_map<string, WindowTypeCache*> WindowMap;
		WindowMap mWindowCache;

		void InitWindowCache(const string& windowType);

		void ClearWindowCaches();
	};
}
#endif
