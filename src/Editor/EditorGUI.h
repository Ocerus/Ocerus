/// @file
/// Handling GUI of editor.
#ifndef _EDITOR_EDITORGUI_H_
#define _EDITOR_EDITORGUI_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"
#include "EntitySystem/ComponentMgr/ComponentID.h"

namespace Editor
{
	/// The EditorGUI class manages the editor GUI. It only manages viewport windows itself, the rest of GUI is
	/// delegated to another classes.
	class EditorGUI
	{
	public:

		/// Constructor.
		EditorGUI();

		/// Destructor.
		~EditorGUI();

		/// Initializes editor GUI.
		void Init();

		/// Deinitializes editor GUI.
		void Deinit();

		/// Updates the GUI.
		void Update(float32 delta);
		
		/// Disables viewports.
		void DisableViewports();

		/// Assigns game and editor cameras to corresponding entities, creates new ones if do not exist.
		void RefreshCameras();

		/// Updates GUI windows.
		void UpdateGUIWindows();

		/// Returns the viewport of the editor window.
		inline GUISystem::ViewportWindow* GetEditorViewport() const { return mEditorViewport; }

		/// Returns the viewport of the game window.
		inline GUISystem::ViewportWindow* GetGameViewport() const { return mGameViewport; }

		/// Returns the editor menu.
		inline EditorMenu* GetEditorMenu() const { return mEditorMenu; }

		/// Returns the entity window.
		inline EntityWindow* GetEntityWindow() const { return mEntityWindow; }

		/// Returns the hierarchy window.
		inline HierarchyWindow* GetHierarchyWindow() const { return mHierarchyWindow; }

		/// Returns the layer window.
		inline LayerWindow* GetLayerWindow() const { return mLayerWindow; }

		/// Returns the prototype window.
		inline PrototypeWindow* GetPrototypeWindow() const { return mPrototypeWindow; }

		/// Returns the resource window.
		inline ResourceWindow* GetResourceWindow() const { return mResourceWindow; }

		/// Name of the entity with the editor camera.
		static const string EditorCameraName;
	
	private:
		/// @name CEGUI Callbacks
		//@{
			/// Handles dropping prototypes on editor viewport.
			bool OnEditorViewportItemDropped(const CEGUI::EventArgs&);
		//@}

		GUISystem::ViewportWindow* mGameViewport;
		GUISystem::ViewportWindow* mEditorViewport;

		EditorMenu* mEditorMenu;
		EntityWindow* mEntityWindow;
		HierarchyWindow* mHierarchyWindow;
		LayerWindow* mLayerWindow;
		PrototypeWindow* mPrototypeWindow;
		ResourceWindow* mResourceWindow;
	};
}

#endif // _EDITOR_EDITORGUI_H_