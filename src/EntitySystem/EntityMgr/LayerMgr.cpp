#include "Common.h"
#include "LayerMgr.h"
#include "ResourceSystem/XMLResource.h"

using namespace EntitySystem;

LayerMgr::LayerMgr() : mLayers(), mDifference(0), mActiveLayerID(0), mList()
{
	ocInfo << "*** LayerMgr init ***";
	PushBackLayer(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "initial_layer").c_str());
}

LayerMgr::~LayerMgr()
{

}

void LayerMgr::LoadLayers(ResourceSystem::XMLNodeIterator& xml)
{
  mLayers.clear();
  mLayerVisibilities.clear();
  
  mDifference = xml.GetAttribute<int32>("Difference");
  mActiveLayerID = xml.GetAttribute<LayerID>("ActiveLayer");
  
  for (ResourceSystem::XMLNodeIterator iter = xml.IterateChildren(); iter != xml.EndChildren(); ++iter)
	{
		if ((*iter).compare("Layer") != 0) continue;
		mLayerVisibilities.push_back(iter.HasAttribute("Visible") ? iter.GetAttribute<bool>("Visible") : true);
		mLayers.push_back(iter.GetChildValue<string>());
	}
  
  if (mLayers.empty())
  { 
    mLayerVisibilities.clear();
    mDifference = 0;
    mActiveLayerID = 0;
    PushBackLayer(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "initial_layer").c_str());
  }
  
  RefreshList();
}

void LayerMgr::SaveLayers(ResourceSystem::XMLOutput& storage)
{
  storage.BeginElementStart("Layers");
  storage.AddAttribute("Difference", StringConverter::ToString(mDifference));
  storage.AddAttribute("ActiveLayer", StringConverter::ToString(mActiveLayerID));
  storage.BeginElementFinish();
  
  Layers::const_iterator lit = mLayers.begin();
  LayerVisibilities::const_iterator lvit = mLayerVisibilities.begin();
  for (; lit != mLayers.end(); ++lit, ++lvit)
	{
		storage.BeginElementStart("Layer");
		storage.AddAttribute("Visible", StringConverter::ToString(*lvit));
		storage.BeginElementFinish();
		storage.WriteString(*lit);
		storage.EndElement();
	}
	
	storage.EndElement();
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
	PushBackLayer(name);
	return GetTopLayerID();
}

LayerID LayerMgr::AddBottomLayer(const string& name)
{
	if (ExistsLayerName(name)) { return 0; }
	PushFrontLayer(name);
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
	mLayerVisibilities.insert(mLayerVisibilities.begin() + (behind + mDifference), true);
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
	mLayerVisibilities.erase(mLayerVisibilities.begin() + (id + mDifference));
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

	if (mActiveLayerID == id)
		mActiveLayerID = 0;

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
	if (id < 0 && behind < 0 && id > behind) { ++behind; }
	else if (id > 0 && behind > 0 && id < behind) { --behind; }
	LayerID newID = InsertAndShift(behind, name);	
	
	// correct the layer of entities in the moved layer and return a new layer ID
	SetLayerOfEntities(newID, toMove);
	if (mActiveLayerID == id)
		mActiveLayerID = newID;
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
	if (mActiveLayerID == id)
		mActiveLayerID = newID;
	return newID;
}

LayerID LayerMgr::MoveLayerUp(LayerID id)
{
	if (id == GetTopLayerID() || !ExistsLayer(id)) { return id; }
	LayerID higherLayerId = id + 1;
	if (id != 0)
	{
		return higherLayerId == GetTopLayerID() ? MoveLayerTop(id) : MoveLayerBehind(id, higherLayerId + 1);
	}
	else
	{
		MoveLayerDown(higherLayerId);
		return 0;
	}
}

LayerID LayerMgr::MoveLayerDown(LayerID id)
{
	if (id != 0)
	{
		return MoveLayerBehind(id, id - 1);
	}
	else
	{
		MoveLayerUp(id - 1);
		return 0;
	}
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

void LayerMgr::MoveEntityUp(EntityHandle entity)
{
	LayerID entityLayer = GetLayerID(entity);
	if (entityLayer != GetTopLayerID())
		SetLayerID(entity, entityLayer + 1);
}

void LayerMgr::MoveEntityDown(EntityHandle entity)
{
	LayerID entityLayer = GetLayerID(entity);
	if (entityLayer != GetBottomLayerID())
		SetLayerID(entity, entityLayer - 1);
}

void LayerMgr::SetActiveLayer(LayerID layerID)
{
	if (!ExistsLayer(layerID))
		layerID = 0;
	mActiveLayerID = layerID;
}

void LayerMgr::SetLayerVisible(LayerID id, bool isVisible)
{
	if (ExistsLayer(id))
		mLayerVisibilities[id + mDifference] = isVisible;
}

void LayerMgr::ToggleLayerVisible(LayerID id)
{
	if (ExistsLayer(id))
		mLayerVisibilities[id + mDifference] = !mLayerVisibilities[id + mDifference];
}

bool LayerMgr::IsLayerVisible(LayerID id) const
{
	return ExistsLayer(id) ? mLayerVisibilities[id + mDifference] : false;
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

void LayerMgr::PushFrontLayer(const string& layerName)
{
	mLayers.push_front(layerName);
	mLayerVisibilities.push_front(true);
}

void LayerMgr::PushBackLayer(const string& layerName)
{
	mLayers.push_back(layerName);
	mLayerVisibilities.push_back(true);
}
