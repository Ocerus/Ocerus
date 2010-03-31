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
		EntityList entities;

		desc.Reset();
		CHECK(gEntityMgr.CreateEntity(desc) == EntityHandle::Null);

		desc.Reset();
		desc.AddComponent(CT_Transform);
		EntityHandle entity1 = gEntityMgr.CreateEntity(desc);
		CHECK(entity1 != EntityHandle::Null);

		desc.SetName("name");
		EntityHandle entity2 = gEntityMgr.CreateEntity(desc);
		CHECK(entity2 != EntityHandle::Null);

		desc.SetName("name");
		EntityHandle entity3 = gEntityMgr.CreateEntity(desc);
		CHECK(entity3 != EntityHandle::Null);

		CHECK(gEntityMgr.EntityExists(entity1));
		CHECK(gEntityMgr.EntityExists(entity2));
		CHECK(gEntityMgr.EntityExists(entity3));

		CHECK(gEntityMgr.FindFirstEntity("name") == entity2);

		gEntityMgr.GetEntitiesWithComponent(entities, CT_Sprite);
		CHECK_EQUAL((size_t)0, entities.size());

		gEntityMgr.GetEntitiesWithComponent(entities, CT_Transform);
		CHECK_EQUAL((size_t)3, entities.size());

		gEntityMgr.GetEntities(entities);
		CHECK_EQUAL((size_t)3, entities.size());

		gEntityMgr.DestroyEntity(entity2);
		gEntityMgr.DestroyEntity(entity1);
		CHECK(gEntityMgr.EntityExists(entity1));
		CHECK(gEntityMgr.EntityExists(entity2));
		gEntityMgr.ProcessDestroyQueue();
		CHECK(!gEntityMgr.EntityExists(entity1));
		CHECK(!gEntityMgr.EntityExists(entity2));

		gEntityMgr.GetEntities(entities);
		CHECK_EQUAL((size_t)1, entities.size());

		gEntityMgr.DestroyAllEntities();
		gEntityMgr.GetEntities(entities);
		CHECK_EQUAL((size_t)0, entities.size());

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