#include "Common.h"
#include "LayerMgr.h"

using namespace EntitySystem;

LayerMgr::LayerMgr() : mLayers(), mDifference(0), mList()
{
	ocInfo << "*** LayerMgr init ***";
	mLayers.push_back("Initial layer"); // TODO: load this string from resource
}

LayerMgr::~LayerMgr()
{

}

inline LayerID GetLayerID(EntityHandle handle)
{
	return gEntityMgr.GetEntityComponentProperty(handle, gEntityMgr.GetEntityComponent
		(handle, CT_Transform), "Layer").GetValue<LayerID>();
}

inline void SetLayerID(EntityHandle handle, LayerID id)
{
	gEntityMgr.GetEntityComponentProperty(handle, gEntityMgr.GetEntityComponent
		(handle, CT_Transform), "Layer").SetValue<LayerID>(id);
}

LayerID LayerMgr::AddTopLayer(const string& name)
{
	if (ExistsLayerName(name)) { return 0; }
	mLayers.push_back(name);
	return GetTopLayerID();
}

LayerID LayerMgr::AddBottomLayer(const string& name)
{
	if (ExistsLayerName(name)) { return 0; }
	mLayers.push_front(name);
	++mDifference;
	return GetBottomLayerID();
}

bool LayerMgr::ExistsLayer(LayerID id) const
{
	return (id + mDifference >= 0) && (id + mDifference < (int32)mLayers.size());
}

bool LayerMgr::ExistsLayerName(const string& name) const
{
	for (Layers::const_iterator it = mLayers.begin(); it != mLayers.end(); ++it)
	{
		if ((*it) == name) { return true; }
	}
	return false;
}

LayerID LayerMgr::GetTopLayerID() const
{
	return mLayers.size() - 1 - mDifference;
}

LayerID LayerMgr::GetBottomLayerID() const
{
	return -mDifference;
}

string LayerMgr::GetLayerName(LayerID id) const
{
	return ExistsLayer(id) ? mLayers[id + mDifference] : "";
}

bool LayerMgr::SetLayerName(LayerID id, const string& name)
{
	if (!ExistsLayer(id)) { return false; }
	if (mLayers[id + mDifference] == name) { return true; }	
	if (ExistsLayerName(name)) { return false; }

	mLayers[id + mDifference] = name;
	return true;
}

LayerID LayerMgr::InsertAndShift(LayerID behind, const string& name)
{
	mLayers.insert(mLayers.begin() + (behind + mDifference), name);
	if (behind <= 0)
	{ 
		++mDifference;
		ShiftEntities(behind - 1, false);
		return behind - 1;
	}
	else
	{ 
		ShiftEntities(behind, true);
		return behind;
	}
}

void LayerMgr::EraseAndShift(LayerID id)
{
	OC_ASSERT(id != 0);
	mLayers.erase(mLayers.begin() + (id + mDifference));
	if (id < 0)
	{ 
		--mDifference;
		ShiftEntities(id - 1, true);
	} else {
		ShiftEntities(id + 1, false);
	}
}

LayerID LayerMgr::InsertLayerBehind(LayerID behind, const string& name)
{
	if (!ExistsLayer(behind) || ExistsLayerName(name)) { return 0; }
	RefreshList();
	return InsertAndShift(behind, name);
}

bool LayerMgr::DeleteLayer(LayerID id, bool destroyEntities)
{
	if (id == 0 || !ExistsLayer(id)) { return false; }
	// remember entities in the layer to delete
	EntityList toDelete;
	GetEntitiesFromLayer(id, toDelete);
	
	// delete the layer and shift the others
	EraseAndShift(id);
	
	// delete or move entites in the layer
	for (EntityList::iterator it = toDelete.begin(); it != toDelete.end(); ++it)
	{
		if (destroyEntities) { gEntityMgr.DestroyEntity(*it); }
		else { SetLayerID(*it, 0); }
	}

	return true;
}

LayerID LayerMgr::MoveLayerBehind(LayerID id, LayerID behind)
{
	if (id == 0 || id == behind || !ExistsLayer(id) || !ExistsLayer(behind)) { return 0; }	
	// remember entites in the layer to move and layer name
	EntityList toMove;
	GetEntitiesFromLayer(id, toMove);
	string name = mLayers.at(id + mDifference);
	
	// delete the layer and shift the others
	EraseAndShift(id);

	// correct the insertion place due to deletion, insert a new layer to layers, shift entities in other layers
	if ((id < 0 && behind < 0 && id > behind) || (id > 0 && behind > 0 && id < behind)) { --behind; }
	LayerID newID = InsertAndShift(behind, name);	
	
	// correct the layer of entities in the moved layer and return a new layer ID
	SetLayerOfEntities(newID, toMove);
	return newID;
}

LayerID LayerMgr::MoveLayerTop(LayerID id)
{
	if (id == 0 || id == GetTopLayerID() || !ExistsLayer(id)) { return 0; }	
	// remember entites in the layer to move and layer name
	EntityList toMove;
	GetEntitiesFromLayer(id, toMove);
	string name = mLayers.at(id + mDifference);
	
	// delete the layer and shift the others
	EraseAndShift(id);

	// add top layer
	LayerID newID = AddTopLayer(name);
	// correct the layer of entities in the moved layer and return a new layer ID
	SetLayerOfEntities(newID, toMove);
	return newID;
}

bool LayerMgr::MoveEntities(LayerID from, LayerID to)
{
	if (!ExistsLayer(from)) { return false; }
	EntityList toMove;
	GetEntitiesFromLayer(from, toMove);
	return SetLayerOfEntities(to, toMove);
}

void LayerMgr::GetEntitiesFromLayer(LayerID id, EntityList& out)
{
	out.clear();
	RefreshList();
	for (EntityList::iterator it = mList.begin(); it != mList.end(); ++it)
	{
		if (GetLayerID(*it) == id) { out.push_back(*it); }
	}
}

bool LayerMgr::SetLayerOfEntities(LayerID id, EntityList& entities)
{
	if (!ExistsLayer(id)) { return false; }
	for (EntityList::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		SetLayerID(*it, id);
	}
	return true;
}

inline void LayerMgr::RefreshList()
{
	gEntityMgr.GetEntitiesWithComponent(mList, CT_Transform);
}

void LayerMgr::ShiftEntities(EntitySystem::LayerID from, bool front)
{
	OC_ASSERT(from != 0);
	bool foreground = from > 0;
	for (EntityList::iterator it = mList.begin(); it != mList.end(); ++it)
	{
		LayerID id = GetLayerID(*it);
		if ((foreground && id >= from) || (!foreground && id <= from))
		{ 
			SetLayerID(*it, id + front ? 1 : -1);
		}
	}
}