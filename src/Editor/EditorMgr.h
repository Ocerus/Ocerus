/// @file
/// Editor entry point.

#ifndef EditorMgr_h__
#define EditorMgr_h__

#include "Base.h"
#include "Singleton.h"
#include "InputSystem/IInputListener.h"
#include "EntitySystem/ComponentMgr/ComponentID.h"

#define gEditorMgr Editor::EditorMgr::GetSingleton()

/// Editor subsystem provides functionality for creating and editing games.
namespace Editor
{
	class EditorGUI;

	class EditorMgr: public Singleton<EditorMgr>
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
		void Update(const float32 delta);

		/// Returns currently selected entity. If no entity is selected,
		/// invalid EntityHandler is returned.
		inline const EntitySystem::EntityHandle* GetCurrentEntity() const;

		/// Sets the currently selected entity to new entity.
		void SetCurrentEntity(const EntitySystem::EntityHandle* newCurrentEntity);

		void UpdateCurrentEntityName(const string& newName);

		void UpdateCurrentEntityProperty(const EntitySystem::ComponentID& componentId, const StringKey propertyKey, const string& newValue);


	private:
		EditorGUI* mEditorGUI;
		EntitySystem::EntityHandle*	mCurrentEntity;
	};

	inline const EntitySystem::EntityHandle* EditorMgr::GetCurrentEntity() const
	{
		return mCurrentEntity;
	}
}


#endif