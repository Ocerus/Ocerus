/// @file
/// Tree logical hierarchy of game entities.
#ifndef _EDITOR_HIERARCHYWINDOW_H
#define _EDITOR_HIERARCHYWINDOW_H

#include "Base.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// Tree logical hierarchy of game entities.
	class HierarchyWindow
	{
	public:

		/// Default constructor.
		HierarchyWindow();

		/// Default destructor.
		~HierarchyWindow();

		/// Initializes the window.
		void Init();

		/// Deinitializes the window.
		void Deinit();

		/// Updates the tree.
		void Update();

		/// Clears the tree.
		void Clear();

		/// Loads the hierarchy from an XML input.
		void LoadHierarchy(ResourceSystem::XMLNodeIterator& xml);

		/// Saves the hierarchy to an XML output.
		void SaveHierarchy(ResourceSystem::XMLOutput& storage);

		/// Adds a new entity to the hierarchy as a parent of the current parent (see SetCurrentParent()).
		void AddEntityToHierarchy(const EntitySystem::EntityHandle toAdd);

		/// Adds a new entity to the hierarchy as a parent of the given entity. If the parent is null the new entity is added
		/// to the root.
		void AddEntityToHierarchy(const EntitySystem::EntityHandle toAdd, const EntitySystem::EntityHandle parent);

		/// Removes an entity from the hierarchy.
		void RemoveEntityFromHierarchy(const EntitySystem::EntityHandle toRemove);

		/// Sets the currently selected item.
		void SetSelectedEntity(EntitySystem::EntityHandle entity);

		/// Returns the hierarchy parent of the given entity. Returns null if no such exists.
		EntitySystem::EntityHandle GetParent(EntitySystem::EntityHandle entity) const;

		/// Sets the current parent in the hierarchy. Entities added in the near future will be parented by this entity.
		void SetCurrentParent(EntitySystem::EntityHandle val) { mCurrentParent = val; }

		/// Clears the hierarchy tree.
		void ClearHierarchy();

		/// Returns the number of entities in the hierarchy tree.
		size_t GetHierarchySize();

		/// Runs a check on the consistency of the hierarchy tree. Returns true if it's ok.
		bool CheckHierarchy();
		
		/// Moves the specified entity before its preceding sibling.
		void MoveUp(EntitySystem::EntityHandle entity);
		
		/// Moves the specified entity after its succeeding sibling.
		void MoveDown(EntitySystem::EntityHandle entity);

		/// Moves the specified entity before its parent.
		void ReparentUp(EntitySystem::EntityHandle entity);

		/// Enables adding of entities to the hierarchy.
		inline void EnableAddEntities() { mDontAddEntities = false; }

		/// Disables adding of entities to the hierarchy.
		inline void DisableAddEntities() { mDontAddEntities = true; }

	private:

		/// @name CEGUI Callbacks
		//@{
		bool OnTreeItemClicked(const CEGUI::EventArgs&);
		bool OnTreeClicked(const CEGUI::EventArgs&);
		bool OnItemDroppedOnTreeItem(const CEGUI::EventArgs&);
		bool OnItemDroppedOnTree(const CEGUI::EventArgs&);
		//@}

		enum ePopupItem
		{
			PI_MOVE_UP = 0,
			PI_MOVE_DOWN,
			PI_REPARENT_UP,
			PI_ADD_ENTITY,
			PI_NEW_COMPONENT,
			PI_DUPLICATE_ENTITY,
			PI_DELETE_ENTITY,
			PI_CREATE_PROTOTYPE
		};

		/// Data item stored in the cached item list.
		struct HierarchyInfo
		{
			uint32 depth;
			EntitySystem::EntityHandle entity;
		};

		typedef hash_map<unsigned int, HierarchyInfo> EntityMap;
		typedef tree<EntitySystem::EntityHandle> HierarchyTree;

		/// Saves the hierarchy recursively.
		void SaveSubtree(ResourceSystem::XMLOutput& storage, const HierarchyTree::iterator_base& parent);

		/// Loads the hierarchy recursively.
		void LoadSubtree(ResourceSystem::XMLNodeIterator& xml, const HierarchyTree::iterator_base& parent);

		/// Creates the subtree based on the saved hierarchy.
		void SetupSubtree(const HierarchyTree::iterator_base& parentIter, const string& hierarchyPath, uint32 depth, size_t& itemIndex);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
		EntityMap mItems;
		HierarchyTree mHierarchy;
		EntitySystem::EntityHandle mCurrentParent;
		bool mDontAddEntities;
		
		void CreatePopupMenu();
		void DestroyPopupMenu();
		void OpenPopupMenu(EntitySystem::EntityHandle clickedEntity, float32 mouseX, float32 mouseY);
		void OnPopupMenuItemClicked(CEGUI::Window* menuItem);

		CEGUI::Window* mPopupMenu;
		CEGUI::Window* mComponentPopupMenu;
		EntitySystem::EntityHandle mCurrentPopupEntity;
		
		/// @name Tree item caching
		//@{

		CEGUI::ItemEntry* CreateTreeItem();
		void SetupTreeItem(CEGUI::ItemEntry* treeItem, EntitySystem::EntityHandle entity, uint32 hierarchyDepth, const string& hierarchyPath);
		void StoreTreeItem(CEGUI::ItemEntry* treeItem);
		CEGUI::ItemEntry* RestoreTreeItem();
		void DestroyTreeItemCache();

		typedef vector<CEGUI::ItemEntry*> TreeItemCache;

		TreeItemCache mTreeItemCache;

		//@}
	};
}

#endif // _EDITOR_HIERARCHYWINDOW_H