#include "Common.h"
#include "CmpShipLogic.h"

using namespace EntitySystem;


void EntitySystem::CmpShipLogic::Init( void )
{

}

void EntitySystem::CmpShipLogic::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpShipLogic::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		GetOwner().SetTeam(GetOwner());
		return EntityMessage::RESULT_OK;
	// this is now done in Game
	/*case EntityMessage::TYPE_DRAW:
		for (EntityList::iterator i=mPlatforms.begin(); i!=mPlatforms.end(); ++i)
			i->PostMessage(EntityMessage::TYPE_DRAW_INNER);
		for (EntityList::iterator i=mLinks.begin(); i!=mLinks.end(); ++i)
			i->PostMessage(EntityMessage::TYPE_DRAW_INNER);
		return EntityMessage::RESULT_OK;*/
	case EntityMessage::TYPE_ADD_PLATFORM:
		assert(msg.data);
		mPlatforms.push_back(*(EntityHandle*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_REMOVE_PLATFORM:
		assert(msg.data);
		{
			EntityHandle platform = *(EntityHandle*)msg.data;
			EntityList::const_iterator platIt = find(mPlatforms.begin(), mPlatforms.end(), platform);
			if (platIt == mPlatforms.end())
				return EntityMessage::RESULT_ERROR;
			mPlatforms.erase(platIt);
			for (EntityList::const_iterator it=mLinks.begin(); it!=mLinks.end(); ++it)
				if (gEntityMgr.PostMessage(*it, EntityMessage(EntityMessage::TYPE_UNLINK_PLATFORM, &platform)) == EntityMessage::RESULT_OK)
				{
					it = mLinks.erase(it);
					if (it == mLinks.end())
						break;
				}
			if (mPlatforms.size() == 0)
			{
				assert(mLinks.size()==0 && "There is nothing to link as there are no platforms");
				gEntityMgr.DestroyEntity(GetOwner());
			}
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DIE:
		{
			for (EntityList::iterator it=mPlatforms.begin(); it!=mPlatforms.end(); ++it)
				it->PostMessage(EntityMessage::TYPE_DIE);
			for (EntityList::iterator it=mLinks.begin(); it!=mLinks.end(); ++it)
				it->PostMessage(EntityMessage::TYPE_DIE);
			gEntityMgr.DestroyEntity(GetOwner());
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_LINK_PLATFORMS:
		assert(msg.data);
		{
			EntityHandle links = *(EntityHandle*)msg.data;
			assert(links.IsValid());
			mLinks.push_back(links);
		}
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipLogic::RegisterReflection()
{

}