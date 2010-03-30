#include "Common.h"
#include "UnitTests.h"

using namespace EntitySystem;

SUITE(EntityMgr)
{
	TEST(EntityManipulation)
	{
		::Test::InitResources();
		::Test::InitEntities();

		EntityDescription desc;
		EntityHandle entity;

		desc.Reset();
		CHECK(gEntityMgr.CreateEntity(desc) == EntityHandle::Null);

		desc.Reset();
		desc.AddComponent(CT_Transform);
		entity = gEntityMgr.CreateEntity(desc);
		CHECK(entity != EntityHandle::Null);

		//TODO pokracovat

		::Test::CleanSubsystems();
	}


	TEST(EntityPersistance)
	{
		::Test::InitResources();
		::Test::InitEntities("entities.xml");

		//TODO

		::Test::CleanSubsystems();
	}
}