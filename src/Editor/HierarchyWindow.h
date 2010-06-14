/// @file
/// Tree logical hierarchy of game entities.

#ifndef HierarchyWindow_h__
#define HierarchyWindow_h__

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

		/// Refreshes the tree.
		inline void Refresh() { RebuildTree(); }

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
		void SetSelectedEntity(const EntitySystem::EntityHandle entity);

		/// Returns the hierarchy parent of the given entity. Returns null if no such exists.
		EntitySystem::EntityHandle GetParent(const EntitySystem::EntityHandle entity) const;

		/// Sets the current parent in the hierarchy. Entities added in the near future will be parented by this entity.
		void SetCurrentParent(const EntitySystem::EntityHandle val) { mCurrentParent = val; }

		/// Runs a check on the consistency of the hierarchy tree. Returns true if it's ok.
		bool CheckHierarchy();

	public:

		/// @name CEGUI Callbacks
		//@{
		bool OnDragContainerMouseButtonDown(const CEGUI::EventArgs&);
		bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		bool OnWindowMouseButtonUp(const CEGUI::EventArgs&);
		bool OnTreeItemDragDropItemDropped(const CEGUI::EventArgs&);
		bool OnTreeDragDropItemDropped(const CEGUI::EventArgs&);
		//@}

	private:

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

		/// Creates the tree based on the saved hierarchy.
		void RebuildTree();

		/// Creates the subtree based on the saved hierarchy.
		void BuildSubtree(const HierarchyTree::iterator_base& parentIter, uint32 depth);

		/// Adds an item to the tree at the given position.
		CEGUI::ItemEntry* AddTreeItem(uint32 index, const string& textName, uint32 depth, const EntitySystem::EntityHandle data);

		/// Adds an item to the tree after the last item.
		CEGUI::ItemEntry* AppendTreeItem(const string& text, uint32 depth, const EntitySystem::EntityHandle data);

		/// Removes an item from the tree at the given position.
		void RemoveTreeItem(uint32 index);

		/// Locates an item in the tree and returns its position. Returns -1 if not found. The depth of the item is returned
		/// in the second parameter.
		int32 FindTreeItem(const EntitySystem::EntityHandle data, uint32& depth);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
		int32 mSelectedIndex;
		EntityMap mItems;
		HierarchyTree mHierarchy;
		EntitySystem::EntityHandle mCurrentParent;
	};
}

#endif // HierarchyWindow_h__