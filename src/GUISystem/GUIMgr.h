/// @file
/// Manager of the GUI system.
#ifndef _GUISYSTEM_GUIMGR_H_
#define _GUISYSTEM_GUIMGR_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"
#include "Utils/Singleton.h"
#include "InputSystem/IInputListener.h"
#include "GfxSystem/IGfxWindowListener.h"

#include <CEGUIString.h>

/// Access the GUIMgr singleton instance.
#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

/// GUI System manages GUI elements in the game and in the editor.
namespace GUISystem
{
	/// The GUIMgr class manages the GUI. The manager depends on InputMgr.
	class GUIMgr: public Singleton<GUIMgr>, public InputSystem::IInputListener, public GfxSystem::IGfxWindowListener
	{
	public:

		/// Constructs a GUIMgr.
		GUIMgr();

		/// Destroys the GUIMgr.
		virtual ~GUIMgr();

		/// Initializes GUI.
		void Init(const string& ceguiLogPath);

		/// Initializes GUI resources.
		void InitResources();

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
        
		/// Loads the system imageset file.
		void LoadSystemImageset(const string& filename);

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

		/// Creates a window of specified type with specified name.
		CEGUI::Window* CreateWindow(const CEGUI::String& type, const CEGUI::String& name);

		/// Destroys the specified window.
		void DestroyWindow(CEGUI::Window* window);

		/// Returns the window of the given name if exists.
		CEGUI::Window* GetWindow(const CEGUI::String& name);

		/// Returns the window of the given name if exists.
		inline CEGUI::Window* GetWindow(const char* name) { return GetWindow(CEGUI::String(name)); }

		/// Returns the window of the given name if exists.
		inline CEGUI::Window* GetWindow(const string& name) { return GetWindow(CEGUI::String(name)); }

		/// Returns true if the window of the given name exists.
		bool WindowExists(const string& name);

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

		/// This method injects resolution change into GUI system. It is part of IGfxWindowListener interface. Called before window resolution is changed.
		virtual void ResolutionChanging(const uint32 width, const uint32 height);

		/// This method injects resolution change into GUI system. It is part of IGfxWindowListener interface. Called after window resolution is changed.
		virtual void ResolutionChanged(const uint32 width, const uint32 height);

		/// Returns the console for printing debug data.
		inline GUIConsole* GetConsole() const { return mGUIConsole; }

		/// Returns the popup menu manager.
		inline PopupMgr* GetPopupMgr() const { return mPopupMgr; }
		
		/// Resource group for GUI resources.
		static const StringKey GUIGroup;

		/// Holds information about an input event.
		struct InputEventInfo
		{
			const InputSystem::KeyInfo* keyInfo;
			const InputSystem::MouseInfo* mouseInfo;
			InputSystem::eMouseButton mouseButton;
		};

		/// Returns the info about the event being currently processed. This is valid only as long as
		/// any of the event handlers is still running.
		inline InputEventInfo& GetCurrentInputEvent() { return mCurrentInputEvent; }

		/// Reloads all layouts if neccessary.
		bool Update();

		/// Reloads all layouts in next call of Update.
		inline void NeedsUpdate() { mNeedsUpdate = true; }

	private:
		/// @name CEGUI related stuff
		//@{
			CEGUI::System* mCEGUI;
			CEGUI::OpenGLRenderer* mRenderer;
			ResourceProvider* mResourceProvider;
			ScriptProvider* mScriptProvider;
		//@}

		/// Input event being currently processed.
		InputEventInfo mCurrentInputEvent;

		/// In-game console
		GUIConsole* mGUIConsole;

		/// Popup menu manager
		PopupMgr* mPopupMgr;

		/// Initializes the basic general stuff.
		void LoadGeneralResources();
		
		/// Initializes the editor stuff.
		void LoadEditorResources();
		
		/// Whether the GUI layouts resources have been modified and the GUI needs update.
		bool mNeedsUpdate;
	};
}
#endif // _GUISYSTEM_GUIMGR_H_