#include "Common.h"
#include "LoadingScreen.h"

using namespace Core;


void Core::LoadingScreen::DoLoading( eType type )
{
	mType = type;

	// set up resource groups
	switch (mType)
	{
	case TYPE_BASIC_RESOURCES:
		break;
	case TYPE_GENERAL_RESOURCES:
		gResourceMgr.AddResourceDirToGroup("engines", "ShipParts");
		gResourceMgr.AddResourceDirToGroup("materials", "ShipParts");
		gResourceMgr.AddResourceDirToGroup("platforms", "ShipParts");
		gResourceMgr.AddResourceDirToGroup("weapons", "ShipParts");
		gResourceMgr.AddResourceDirToGroup("ammo", "ShipParts");
		gResourceMgr.AddResourceDirToGroup("ships", "Ships");
		gResourceMgr.AddResourceFileToGroup("water.png", "Backgrounds");	
		gResourceMgr.AddResourceDirToGroup("xml", "xml");
		gResourceMgr.AddResourceDirToGroup("psi", "psi");

		gResourceMgr.LoadResourcesInGroup("Backgrounds");
		gResourceMgr.LoadResourcesInGroup("ShipParts");
		gResourceMgr.LoadResourcesInGroup("Ships");
		gResourceMgr.LoadResourcesInGroup("psi");

		gGUIMgr.LoadStyle();
		break;
	}
}

void Core::LoadingScreen::Init()
{
	if (mType != TYPE_BASIC_RESOURCES)
	{
		gResourceMgr.SetLoadingListener(this);

		// init loading screen gfx

	}
}

void Core::LoadingScreen::Deinit()
{

}

void Core::LoadingScreen::Draw()
{
	if (mType == TYPE_BASIC_RESOURCES) // can't draw when we don't have the gui ready
		return;

	if (gGfxRenderer.BeginRendering())
	{
		gGfxRenderer.ClearScreen(GfxSystem::Color(255,0,0));
		gGfxRenderer.EndRendering();
	}
}

void Core::LoadingScreen::ResourceGroupLoadStarted( const string& groupName, uint32 resourceCount )
{

	Draw();
}

void Core::LoadingScreen::ResourceLoadStarted( const ResourceSystem::ResourcePtr& resource )
{

	Draw();
}

void Core::LoadingScreen::ResourceLoadEnded( void )
{

	Draw();
}

void Core::LoadingScreen::ResourceGroupLoadEnded( void )
{

	Draw();
}

