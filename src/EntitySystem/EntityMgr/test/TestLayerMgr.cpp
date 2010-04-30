#include "Common.h"
#include "UnitTests.h"
#include "../LayerMgr.h"

using namespace EntitySystem;

SUITE(LayerMgr)
{
	TEST(Init)
	{
		::Test::InitEntities();
		StringSystem::StringMgr::Init();
		LayerMgr::CreateSingleton();

		CHECK(gLayerMgr.ExistsLayer(0));
	}


	TEST(PushLayers)
	{
		gLayerMgr.AddTopLayer("First foreground");
		CHECK_EQUAL(gLayerMgr.GetTopLayerID(), 1);
		CHECK_EQUAL(gLayerMgr.GetLayerName(1), "First foreground");
		gLayerMgr.AddBottomLayer("First background");
		CHECK_EQUAL(gLayerMgr.GetBottomLayerID(), -1);
		CHECK_EQUAL(gLayerMgr.GetLayerName(-1), "First background");
	}

	TEST(SetLayerName)
	{
		LayerID id = gLayerMgr.AddTopLayer("Really second foreground");
		CHECK_EQUAL(gLayerMgr.SetLayerName(id, "First foreground"), false);
		CHECK(gLayerMgr.SetLayerName(id, "Second foreground"));
		CHECK_EQUAL(gLayerMgr.GetLayerName(id), "Second foreground");
	}

	TEST(InsertLayer)
	{
		LayerID id = gLayerMgr.InsertLayerBehind(gLayerMgr.GetTopLayerID(), "Really second foreground");
		CHECK_EQUAL(id, gLayerMgr.GetTopLayerID() - 1);
	}

	TEST(MoveLayer)
	{
		LayerID id = gLayerMgr.AddBottomLayer("Second background"); 
		LayerID topID = gLayerMgr.GetTopLayerID(); 
		LayerID newID = gLayerMgr.MoveLayerBehind(topID - 1, id + 1); 
		CHECK_EQUAL(newID, id);
		CHECK_EQUAL(topID, gLayerMgr.GetTopLayerID() + 1);
	}

	TEST(MoveLayer2)
	{
		LayerID bottomID = gLayerMgr.GetBottomLayerID();
		LayerID newID = gLayerMgr.MoveLayerBehind(bottomID + 1, 0);
		CHECK_EQUAL(newID, -1);
		CHECK_EQUAL(bottomID, gLayerMgr.GetBottomLayerID());
	}

	TEST(DeleteLayer)
	{
		CHECK_EQUAL(gLayerMgr.DeleteLayer(0, false), false);
		LayerID bottomID = gLayerMgr.GetBottomLayerID();
		LayerID topID = gLayerMgr.GetTopLayerID();
		CHECK(gLayerMgr.DeleteLayer(-1, false));
		CHECK_EQUAL(bottomID, gLayerMgr.GetBottomLayerID() - 1);
		CHECK(gLayerMgr.DeleteLayer(topID, false));
		CHECK_EQUAL(topID, gLayerMgr.GetTopLayerID() + 1);
	}

	TEST(MoveLayer3)
	{
		LayerID layer1ID = gLayerMgr.AddBottomLayer("Layer1");
		gLayerMgr.AddBottomLayer("Layer2");

		CHECK_EQUAL("Layer1", gLayerMgr.GetLayerName(layer1ID));
		CHECK_EQUAL("Layer2", gLayerMgr.GetLayerName(layer1ID - 1));

		LayerID newLayer1 = gLayerMgr.MoveLayerBehind(layer1ID, layer1ID - 1);

		CHECK_EQUAL("Layer1", gLayerMgr.GetLayerName(newLayer1));
		CHECK_EQUAL("Layer2", gLayerMgr.GetLayerName(newLayer1 + 1));
	}

	TEST(MoveLayer4)
	{
		LayerID layerID = 0;
		CHECK(gLayerMgr.ExistsLayer(layerID));

		CHECK_EQUAL(0, gLayerMgr.MoveLayerBehind(layerID, gLayerMgr.GetBottomLayerID()));
	}

	TEST(MoveLayer5)
	{
		LayerID layerID = -1;
		string layerName = gLayerMgr.GetLayerName(layerID);
		LayerID newLayerID = gLayerMgr.MoveLayerUp(layerID);
		CHECK_EQUAL(1, newLayerID);
		CHECK_EQUAL(layerName, gLayerMgr.GetLayerName(1));
	}

	TEST(InvariantTest)
	{
		CHECK(gLayerMgr.ExistsLayer(0));
	}


	TEST(Clean)
	{
		::Test::CleanSubsystems();
	}
}