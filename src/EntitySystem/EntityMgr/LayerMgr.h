/// @file
/// Layer manager for editor.

#ifndef LayerMgr_h__
#define LayerMgr_h__

#include "Base.h"
#include "Singleton.h"
#include "EntitySystem/ComponentMgr/ComponentID.h"

#define gLayerMgr EntitySystem::LayerMgr::GetSingleton()

namespace EntitySystem
{
	/// Unique identifier of a layer of entities.
	typedef int32 LayerID;
	
	/// Manages the layers for entities in editor.
	class LayerMgr: public Singleton<LayerMgr>
	{
	public:

		/// Constructor.
		LayerMgr();

		/// Destructor.
		virtual ~LayerMgr();

		/// Clears all data.
		void Clear();
		
		/// Loads layers from an XML input.
		void LoadLayers(ResourceSystem::XMLNodeIterator& xml);

		/// Saves layers to an XML output.
		void SaveLayers(ResourceSystem::XMLOutput& storage);

		/// Adds the layer top.
		/// @return ID of the new layer, 0 in case of error.
		LayerID AddTopLayer(const string& name);

		/// Adds the layer bottom.
		/// @return ID of the new layer, 0 in case of error.
		LayerID AddBottomLayer(const string& name);

		/// Returns whether the layer exists.
		bool ExistsLayer(LayerID id) const;

		/// Returns whether exists a layer with this name.
		bool ExistsLayerName(const string& name) const;

		/// Returns ID of the layer specified by name.
		LayerID GetLayerID(const string& name) const;

		/// Returns the ID of the top layer.
		LayerID GetTopLayerID() const;

		/// Returns the ID of the bottom layer.
		LayerID GetBottomLayerID() const;

		/// Inserts the layer behind the specified layer.
		/// Use AddTopLayer to add the top layer instead.
		/// @param behind The layer that the new layer is placed behind.
		/// @param name A name of the new layer.
		/// @return ID of the new layer, 0 in case of error.
		LayerID InsertLayerBehind(LayerID behind, const string& name);

		/// Moves the layer behind the specified layer.
		/// Use MoveLayerTop to move layer top.
		/// @return A new ID of the moved layer, 0 in case of error.
		LayerID MoveLayerBehind(LayerID id, LayerID behind);

		/// Moves the layer above the specified layer.
		/// @return A new ID of the moved layer, 0 in case of error.
		LayerID MoveLayerAbove(LayerID id, LayerID above);

		/// Moves the layer top.
		/// @return A new ID of the moved layer, 0 in case of error.
		LayerID MoveLayerTop(LayerID id);

		/// Moves the layer one position up, or does nothing, if it is already on the top.
		/// @return New ID of the moved layer.
		LayerID MoveLayerUp(LayerID id);

		/// Moves the layer one position down, or does nothing, if it is already on the bottom.
		/// @return New ID of the moved layer.
		LayerID MoveLayerDown(LayerID id);

		/// Moves the entites from one layer to another one.
		bool MoveEntities(LayerID from, LayerID to);

		/// Returns whether entity has a layer.
		bool EntityHasLayer(EntityHandle entity) const;

		/// Gets layer ID of given entity.
		LayerID GetEntityLayerID(EntityHandle entity) const;

		/// Sets layer ID of given entity.
		void SetEntityLayerID(EntityHandle handle, LayerID id) const;

		/// Deletes the layer.
		/// @param id The layer to be deleted.
		/// @param destroyEntities Whether to destroy entities in the layer or to move them to initial layer.
		/// @return If the layer was deleted.
		bool DeleteLayer(LayerID id, bool destroyEntities);

		/// Gets layer name from ID.
		string GetLayerName(LayerID id) const;

		/// Sets layer name with ID and returns if it is successful (it must be a unique name).
		bool SetLayerName(LayerID id, const string& name);

		/// Retrieves entities from a specified layer.
		void GetEntitiesFromLayer(LayerID id, EntityList& out);

		/// Set the layer of specified entities.
		bool SetLayerOfEntities(LayerID id, EntityList& entities);

		/// Moves the specified entity to higher layer.
		void MoveEntityUp(EntityHandle entity);

		/// Moves the specified entity to lower layer.
		void MoveEntityDown(EntityHandle entity);

		/// Sets the active layer to the specified layer.
		void SetActiveLayer(LayerID id);

		/// Gets the active layer.
		inline LayerID GetActiveLayer() const { return mActiveLayerID; }

		/// Returns whether the specified layer is visible.
		bool IsLayerVisible(LayerID id) const;

		/// Sets the visibility of the specified layer.
		void SetLayerVisible(LayerID id, bool isVisible);

		/// Toggle the visibility of the specified layer.
		void ToggleLayerVisible(LayerID);

	private:

		typedef deque<string> Layers;
		typedef deque<bool> LayerVisibilities;

		/// Container of layers.
		Layers mLayers;

		/// Container of the layers' visibility flags
		LayerVisibilities mLayerVisibilities;

		/// Difference between index in mLayers and LayerID.
		int32 mDifference;

		/// Current layer
		LayerID mActiveLayerID;

		/// Cached list of entities.
		EntityList mList;

		/// Refreshes the entity list from EntityMgr.
		inline void RefreshList();
		
		/// Moves the entities of foreground (positive ID) or background {negative ID) layers one layer
		/// front or back begining on specified layer.
		/// @param from From which layer the layers will be shift (i.e. from = -3 -> layer -3, -4, -5 ... will be shifted)
		/// @param front Whether the layers will be shift front (ID += 1) or back (ID -= 1)
		void ShiftEntities(LayerID from, bool front);

		/// Erases the layer from layers and shift entities in other layers.
		/// Do nothing with entities in erased layer and do not refresh an entity list.
		void EraseAndShift(LayerID id);

		/// Inserts a new layer after specified layer with specified name and return the new ID.
		/// Do not refresh an entity list.
		LayerID InsertAndShift(LayerID behind, const string& name);

		/// Appends a new layer to the layer container.
		void PushFrontLayer(const string& layerName);

		/// Prepends a new layer to the layer container.
		void PushBackLayer(const string& layerName);
	};
}


#endif