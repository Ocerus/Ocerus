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

		/// Returns currently selected entity. If no entity is selected,
		/// invalid EntityHandler is returned.
		inline const EntitySystem::EntityHandle GetCurrentEntity() const;

		/// Sets the currently selected entity to new entity.
		void SetCurrentEntity(const EntitySystem::EntityHandle newCurrentEntity);

		/// Sets a new name for the selected entity.
		void UpdateCurrentEntityName(const string& newName);

		/// Sets a new value to a property of the selected entity.
		void UpdateCurrentEntityProperty(const EntitySystem::ComponentID& componentId, const StringKey propertyKey, const string& newValue);

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

		/// Creates a new entity.
		void CreateEntity();

		/// Duplicates current entity.
		void DuplicateEntity();

		/// Delete current entity.
		void DeleteEntity();
		
		/// Adds a new component of componentType to current entity.
		void AddComponent(EntitySystem::eComponentType componentType);

		/// Removes the component with given componentId from current entity.
		void RemoveComponent(const EntitySystem::ComponentID& componentId);
		
		
	private:
		EditorGUI* mEditorGUI;
		EntitySystem::EntityHandle mCurrentEntity;
	};

	inline const EntitySystem::EntityHandle EditorMgr::GetCurrentEntity() const
	{
		return mCurrentEntity;
	}
}


#endif