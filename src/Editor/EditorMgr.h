/// @file
/// Editor entry point.

#ifndef EditorMgr_h__
#define EditorMgr_h__

#include "Base.h"
#include "Singleton.h"
#include "InputSystem/IInputListener.h"
#include "EntitySystem/ComponentMgr/ComponentID.h"
#include "EntitySystem/ComponentMgr/ComponentEnums.h"
#include "InputSystem/IInputListener.h"

/// Macro for easier use.
#define gEditorMgr Editor::EditorMgr::GetSingleton()

/// Editor subsystem provides functionality for creating and editing games.
namespace Editor
{
	///@TODO doxygen
	class EditorMgr: public Singleton<EditorMgr>, public InputSystem::IInputListener
	{
	public:

		/// Creates an EditorMgr instance.
		EditorMgr();

		/// Destroys the EditorMgr instance.
		virtual ~EditorMgr();

		/// Initializes the editor.
		void Init();

		/// Deinitializes the editor.
		void Deinit();

		/// Updates the editor.
		void Update(float32 delta);

		/// Draws the editor viewports.
		void Draw(float32 delta);

		/// Clear selections and sets initial time.
		void Reset();

		/// @name Methods related to current and selected entity.
		//@{

			/// Returns currently edited entity. If no entity is edited,
			/// invalid EntityHandler is returned.
			inline const EntitySystem::EntityHandle GetCurrentEntity() const;

			/// Sets the currently edited entity.
			void SetCurrentEntity(const EntitySystem::EntityHandle newCurrentEntity);

			/// Returns currently selected entity. If no entity is selected, invalid EntityHandler is returned.
			/// If there are more selected entities only one of them is chosen.
			inline const EntitySystem::EntityHandle GetSelectedEntity() const;

			/// Selects an entity in the editor window.
			void SelectEntity(const EntitySystem::EntityHandle entity);

			/// Returns true if the entity is in the current selection.
			bool IsEntitySelected(const EntitySystem::EntityHandle entity) const;

			/// Clears all selected entities.
			void ClearSelection();

		//@}
			
		/// Adds a new popup menu to the list.
		void RegisterPopupMenu(PopupMenu* menu);

		/// Removes the popup menu from the list.
		void UnregisterPopupMenu(PopupMenu* menu);

		/// Disables closing of popup menus.
		inline void DisablePopupClosing() { mPopupClosingEnabled = false; }

		/// Enables closing of popup menus.
		inline void EnablePopupClosing() { mPopupClosingEnabled = true; }

		/// Closes all popup menus.
		void CloseAllPopupMenus();

		/// True if the editor is editing a prototype.
		bool IsEditingPrototype() const;

		/// @name GUI-related classes accessors
		//@{
			/// Returns the viewport of the editor window.
			GUISystem::ViewportWindow* GetEditorViewport() const;

			/// Returns the viewport of the game window.
			GUISystem::ViewportWindow* GetGameViewport() const;

			/// Returns the EditorGUI.
			inline EditorGUI* GetEditorGUI() const { return mEditorGUI; }

			/// Returns the editor menu.
			EditorMenu* GetEditorMenu() const;

			/// Returns the entity window.
			EntityWindow* GetEntityWindow() const;

			/// Returns the hierarchy window.
			HierarchyWindow* GetHierarchyWindow() const;

			/// Returns the layer window.
			LayerWindow* GetLayerWindow() const;

			/// Returns the prototype window.
			PrototypeWindow* GetPrototypeWindow() const;

			/// Returns the resource window.
			ResourceWindow* GetResourceWindow() const;
		//@}
		
		/// Returns the ProjectMgr.
		inline Core::Project* GetCurrentProject() const { return mCurrentProject; }
		
		/// Returns whether there is an opened project.
		bool IsProjectOpened() const;

		
		void OnProjectOpened();
		
		void OnProjectClosed();
		
		void OnSceneOpened();
		
		void OnSceneClosed();

		/// Refreshes the prototype window.
		void UpdatePrototypeWindow();

		/// Refreshes the resource window.
		void UpdateResourceWindow();

		/// Updates the scenes in scene menu.
		void UpdateSceneMenu();

		/// Updates the enabled/disabled state of items in menu.
		void UpdateMenuItemsEnabled();

		/// Shows dialog for "Create new project"
		void ShowCreateProjectDialog();

		/// Creates a new project in specified path.
		void CreateProject(const string& projectPath);
		
		/// Shows dialog for "Open project".
		void ShowOpenProjectDialog();
		
		/// Opens the project specified with given path.
		void OpenProject(const string& projectPath);

		/// Closes the opened project.
		void CloseProject();

		/// Saves opened scene.
		void SaveOpenedScene();

		/// Shows dialog for "New scene".
		void ShowNewSceneDialog();

		/// Creates new scene.
		void CreateScene(const string& sceneFilename, const string& sceneName);

		/// Shows dialog for "Quit".
		void ShowQuitDialog();

		/// Resumes action of the edited game.
		void ResumeAction();

		/// Pauses action of the edited game until it is resumed again.
		void PauseAction();

		/// Restarts the edited game action to the initial (saved) state.
		void RestartAction();

		/// Returns true if the input is locked to the game window.
		bool IsLockedToGame() const;

		/// The type of edit tool.
		enum eEditTool { ET_MOVE=0, ET_ROTATE, ET_ROTATE_Y, ET_SCALE };

		/// The type of the action tool.
		enum eActionTool { AT_RUN=0, AT_PAUSE, AT_RESTART };

		/// Switches the edit tool to the new one. It also changes the currently pressed button.
		void SwitchEditTool(eEditTool newTool);

		/// Switches the action tool to the new one. It also changes the currently pressed button.
		void SwitchActionTool(eActionTool newTool);

		/// Handles key shortcuts related to the editor. Returns true if the key was handled, false otherwise.
		bool HandleShortcuts(InputSystem::eKeyCode keyCode);

		
	public:

   		/// @name Callbacks from InputSystem::IInputListener
		//@{

		virtual bool KeyPressed(const InputSystem::KeyInfo& ke);

		virtual bool KeyReleased(const InputSystem::KeyInfo& ke);

		virtual bool MouseMoved(const InputSystem::MouseInfo& mi);

		virtual bool MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		virtual bool MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		//@}

		/// If the mouse cursor is currently above the editor window and the window is active the function returns true
		/// and fills the output parameter with the converted cursor position. Otherwise it returns false.
		bool GetWorldCursorPos(Vector2& worldPos) const;

		/// Called by the value editors when their value changes.
		void PropertyValueChanged();

		/// Creates a new entity.
		void CreateEntity(const string& name);

		/// Duplicates the current entity.
		void DuplicateCurrentEntity();

		/// Deletes the current entity.
		void DeleteCurrentEntity();

		/// Creates a prototype from the current entity and links it to the prototype.
		void CreatePrototypeFromCurrentEntity();

		/// Duplicates selected entities.
		void DuplicateSelectedEntities();

		/// Deletes selected entities.
		void DeleteSelectedEntities();
		
		/// Adds a new component of componentType to current entity.
		void AddComponent(EntitySystem::eComponentType componentType);

		/// Removes the component with given componentId from current entity.
		void RemoveComponent(const EntitySystem::ComponentID& componentId);

		/// Sets the current edit tool.
		void SetCurrentEditTool(eEditTool newEditTool);

		/// Handles held key shortcuts (i.e. for camera movement)
		void HandleHeldShortcuts( float32 delta );

	private:

		EditorGUI* mEditorGUI;
		Core::Project* mCurrentProject;
		list<PopupMenu*> mPopupMenus;
		bool mPopupClosingEnabled;
		EntitySystem::EntityHandle mCurrentEntity; ///< Currently edited entity in the components' window.

		// Selections stuff.
		bool mMousePressedInSceneWindow; ///< True if the mouse button was pressed in the scene editor's window.
		bool mMultiselectStarted; ///< True if the user started a multi-selection mode.
		Vector2 mSelectionCursorPosition; ///< World position where the selection started.
		EntitySystem::EntityHandle mHoveredEntity; ///< Entity the mouse is currently hovering over.
		typedef vector<EntitySystem::EntityHandle> EntityList;
		EntityList mSelectedEntities; ///< Currently selected entities.

		// Tools.
		eEditTool mEditTool; ///< Current tool.
		bool mEditToolWorking; ///< If true the tool is currently doing something.
		Vector2 mEditToolCursorPosition; ///< World position where the tool started.
		vector<Vector2> mEditToolBodyPositions; ///< Initial positions of the bodies.
		vector<float32> mEditToolBodyAngles; ///< Initial angle of the bodies.
		vector<Vector2> mEditToolBodyScales; ///< Initial scale of the bodies.
		bool mIsInitialTime; ///< Whether the scene is in initial time (before the first call of ResumeAction or after the call of RestartAction).


		/// Draws the shape of the entity using the selected color.
		bool DrawEntityPhysicalShape(const EntitySystem::EntityHandle entity, const GfxSystem::Color shapeColor, const GfxSystem::Color fillColor, const float32 shapeWidth);

		/// Processes the currently selected edit tool.
		void ProcessCurrentEditTool(const GfxSystem::Point& screenCursorPos);

		//Shortcuts
		KeyShortcuts* mShortcuts;
		
		//Dialogs
		CreateProjectDialog* mCreateProjectDialog;
	
	};

	inline const EntitySystem::EntityHandle EditorMgr::GetCurrentEntity() const
	{
		return mCurrentEntity;
	}

	inline const EntitySystem::EntityHandle EditorMgr::GetSelectedEntity() const
	{
		if (mSelectedEntities.size() == 0) return EntitySystem::EntityHandle::Null;
		else return mSelectedEntities[0];
	}
}


#endif