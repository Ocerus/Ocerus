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
	class EditorMgr: public Singleton<EditorMgr>, public InputSystem::IInputListener
	{
	public:

		/// Constructor.
		EditorMgr();

		/// Destructor.
		virtual ~EditorMgr();

		/// Loads editor.
		void LoadEditor();

		/// Unloads editor.
		void UnloadEditor();

		/// Updates the editor.
		void Update(float32 delta);

		/// Draws the editor viewports.
		void Draw(float32 delta);

		/// Returns currently edited entity. If no entity is edited,
		/// invalid EntityHandler is returned.
		inline const EntitySystem::EntityHandle GetCurrentEntity() const;

		/// Sets the currently edited entity.
		void SetCurrentEntity(const EntitySystem::EntityHandle newCurrentEntity);

		/// Returns currently selected entity. If no entity is selected, invalid EntityHandler is returned.
		/// If there are more selected entities only one of them is chosen.
		inline const EntitySystem::EntityHandle GetSelectedEntity() const;

		/// Sets a new name for the selected entity.
		void UpdateCurrentEntityName(const string& newName);

		/// Sets a new value to a property of the selected entity.
		void UpdateCurrentEntityProperty(const EntitySystem::ComponentID& componentId, const StringKey propertyKey, const string& newValue);

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

	protected:

		friend class EditorGUI;
		friend class EditorMenu;

		/// The type of edit tool.
		enum eEditTool { ET_MOVE=0, ET_ROTATE, ET_ROTATE_Z, ET_SCALE };

		/// Creates a new entity.
		void CreateEntity();

		/// Duplicates the current entity.
		void DuplicateEntity();

		/// Deletes the current entity.
		void DeleteEntity();

		/// Duplicates selected entities.
		void DuplicateSelectedEntities();

		/// Deletes selected entities.
		void DeleteSelectedEntities();
		
		/// Adds a new component of componentType to current entity.
		void AddComponent(EntitySystem::eComponentType componentType);

		/// Removes the component with given componentId from current entity.
		void RemoveComponent(const EntitySystem::ComponentID& componentId);

		/// Resumes action of the edited game.
		void ResumeAction();

		/// Pauses action of the edited game until it is resumed again.
		void PauseAction();

		/// Restarts the edited game action to the initial (saved) state.
		void RestartAction();

		/// Sets the current edit tool.
		void SetEditTool(eEditTool newEditTool);

	private:

		EditorGUI* mEditorGUI;
		list<PopupMenu*> mPopupMenus;
		bool mPopupClosingEnabled;
		EntitySystem::EntityHandle mCurrentEntity; ///< Currently edited entity in the components' window.

		// Selections stuff.
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


		/// Draws the shape of the entity using the selected color.
		bool DrawEntityPhysicalShape(const EntitySystem::EntityHandle entity, const GfxSystem::Color shapeColor, const float32 shapeWidth);

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