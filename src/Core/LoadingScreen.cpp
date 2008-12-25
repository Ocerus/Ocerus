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
		gResourceMgr.AddResourceFileToGroup("water.png", "Backgrounds");	
		gResourceMgr.AddResourceDirToGroup("xml", "xml");
		gResourceMgr.AddResourceDirToGroup("psi", "psi");

		gResourceMgr.AddResourceFileToGroup("gui/schemes/TaharezLook.scheme", "schemes");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/TaharezLook.imageset", "imagesets");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/TaharezLook.tga", "imagesets"); 
		gResourceMgr.AddResourceFileToGroup("gui/looknfeel/TaharezLook.looknfeel", "looknfeels");
		gResourceMgr.AddResourceFileToGroup("gui/fonts/Commonwealth-10.font", "fonts");
		gResourceMgr.AddResourceFileToGroup("gui/fonts/Commonv2c.ttf", "fonts");
		gResourceMgr.AddResourceFileToGroup("gui/schemes/Console.scheme", "schemes");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/Console.imageset", "imagesets");
		gResourceMgr.AddResourceFileToGroup("gui/imagesets/BSLogov2.png", "imagesets");
		gResourceMgr.AddResourceFileToGroup("gui/layouts/Console.layout", "layouts");

		gResourceMgr.LoadResourcesInGroup("Backgrounds");
		gResourceMgr.LoadResourcesInGroup("ShipParts");
		gResourceMgr.LoadResourcesInGroup("xml");
		gResourceMgr.LoadResourcesInGroup("psi");
		gResourceMgr.LoadResourcesInGroup("fonts");
		gResourceMgr.LoadResourcesInGroup("schemes");
		gResourceMgr.LoadResourcesInGroup("imagesets");
		gResourceMgr.LoadResourcesInGroup("looknfeels");
		gResourceMgr.LoadResourcesInGroup("layouts");

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

