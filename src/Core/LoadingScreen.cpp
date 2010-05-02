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

		// basic resources needed by the program
		gResourceMgr.AddResourceFileToGroup("general/NullTexture.png", "General", ResourceSystem::RESTYPE_TEXTURE, ResourceSystem::BPT_SYSTEM, "NullTexture");

		// make sure we have the necessary resources
		if (!gResourceMgr.GetResource("General", "NullTexture")) OC_FAIL("Some essential resources are missing!");

		// load the basic resources into memory
		gResourceMgr.LoadResourcesInGroup("General");


		// additional resources common for all projects
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, ".", "Scripts", ".+\\.as");

		gResourceMgr.LoadResourcesInGroup("Scripts");

		// prototypes
		gEntityMgr.LoadPrototypes();

		gGUIMgr.Init();

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

	//TODO:Gfx
	/*if (gGfxRenderer.BeginRendering())
	{
		gGfxRenderer.ClearScreen(GfxSystem::Color(255,0,0));
		gGfxRenderer.EndRendering();
	}*/
}

void Core::LoadingScreen::ResourceGroupLoadStarted( const string& groupName, uint32 resourceCount )
{
	OC_UNUSED(groupName);
	OC_UNUSED(resourceCount);
	Draw();
}

void Core::LoadingScreen::ResourceLoadStarted( const ResourceSystem::ResourcePtr& resource )
{
	OC_UNUSED(resource);
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

