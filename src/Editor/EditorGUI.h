/// @file
/// Handling GUI of editor.

#ifndef _EDITORGUI_H_
#define _EDITORGUI_H_

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"
#include "EntitySystem/ComponentMgr/ComponentID.h"

namespace Editor
{
	/// The EditorGUI class manages the editor GUI.
	class EditorGUI
	{
	public:

		/// Constructor.
		EditorGUI();

		/// Destructor.
		~EditorGUI();

		/// Loads editor GUI.
		void LoadGUI();

		/// Updates the GUI.
		void Update(float32 delta);

		/// Draws viewports managed by Editor
		void Draw(float32 delta);
		
		/// Updates the entity editor.
		void UpdateEntityEditorWindow();

		/// Clears the entity editor.
		void ClearEntityEditorWindow();
		
		/// Updates the entity editor in the following call of Update.
		void NeedUpdateEntityEditorWindow() { mNeedEntityEditorUpdate = true; }

		/// Disables viewports.
		void DisableViewports();

		/// Assigns game and editor cameras to corresponding entities, creates new ones if do not exist.
		void RefreshCameras();

		/// Returns the viewport of the editor window.
		inline GUISystem::ViewportWindow* GetEditorViewport() const { return mEditorViewport; }

		/// Returns the viewport of the game window.
		inline GUISystem::ViewportWindow* GetGameViewport() const { return mGameViewport; }

		/// Returns the resource window.
		inline ResourceWindow* GetResourceWindow() const { return mResourceWindow; }

		/// Returns the prototype window.
		inline PrototypeWindow* GetPrototypeWindow() const { return mPrototypeWindow; }

		/// Returns the hierarchy window.
		inline HierarchyWindow* GetHierarchyWindow() const { return mHierarchyWindow; }

		/// Returns the layer window.
		inline LayerWindow* GetLayerWindow() const { return mLayerWindow; }

		///@TODO doxygen
		inline EditorMenu* GetEditorMenu() const { return mEditorMenu; }

		void AddWidgetToTabNavigation(CEGUI::Window* widget);

		/// @name CEGUI Callbacks
		//@{
		bool OnComponentRemoveClicked(const CEGUI::EventArgs&);
		//@}

	public:

		/// Name of the entity with the editor camera.
		static const string EditorCameraName;

		/// Callback from CEGUI.
		bool OnEditorViewportItemDropped(const CEGUI::EventArgs&);

	private:
		/// Recursively subscribes every menu item to the mouse enters/leaves/clicked events
		void ConfigureMenu(CEGUI::Window* parent, bool isMenubar);
		
		int32 mPropertyItemHeight;
		int32 mComponentGroupHeight;
		float32 mPropertyUpdateTimer;
		bool mNeedEntityEditorUpdate;

		typedef vector<AbstractValueEditor*> PropertyEditors;
		PropertyEditors mPropertyEditors;
		typedef vector<GUISystem::VerticalLayout*> VerticalLayouts;
		VerticalLayouts mVerticalLayouts;
		GUISystem::VerticalLayout* mEntityEditorLayout;
		GUISystem::ViewportWindow* mGameViewport;
		GUISystem::ViewportWindow* mEditorViewport;
		GUISystem::TabNavigation* mTabNavigation;
		EditorMenu* mEditorMenu;
		LayerWindow* mLayerWindow;
		ResourceWindow* mResourceWindow;
		PrototypeWindow* mPrototypeWindow;
		HierarchyWindow* mHierarchyWindow;
	};
}

#endif // _EDITORGUI_H_