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

		/// Adds a new entity to the hierarchy as a parent of the given entity. If the parent is null the new entity is added
		/// to the root.
		void AddEntityToHierarchy(const EntitySystem::EntityHandle parent, const EntitySystem::EntityHandle toAdd);

		/// Removes an entity from the hierarchy.
		void RemoveEntityFromHierarchy(const EntitySystem::EntityHandle toRemove);

	public:

		/// @name CEGUI Callbacks
		//@{
		bool OnDragContainerMouseButtonUp(const CEGUI::EventArgs&);
		bool OnWindowMouseButtonUp(const CEGUI::EventArgs&);
		//@}

	private:

		typedef hash_map<unsigned int, EntitySystem::EntityHandle> EntityMap;
		typedef tree<EntitySystem::EntityHandle> HierarchyTree;

		/// Creates the tree based on the saved hierarchy.
		void RebuildTree();

		/// Creates the subtree based on the saved hierarchy.
		void BuildSubtree(const HierarchyTree::iterator_base& parentIter, uint32 depth);

		/// Adds an item to the tree at the given position.
		CEGUI::ItemEntry* AddTreeItem(uint32 index, const string& text, const EntitySystem::EntityHandle data);

		/// Adds an item to the tree after the last item.
		CEGUI::ItemEntry* AppendTreeItem(const string& text, const EntitySystem::EntityHandle data);

		/// Removes an item from the tree at the given position.
		void RemoveTreeItem(uint32 index);

		CEGUI::Window* mWindow;
		CEGUI::ItemListbox* mTree;
		int32 mSelectedIndex;
		EntityMap mItems;
		HierarchyTree mHierarchy;
	};
}

#endif // HierarchyWindow_h__