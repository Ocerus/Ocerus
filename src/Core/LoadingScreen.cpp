#include "Common.h"
#include "LoadingScreen.h"
#include "GfxSystem/Texture.h"
#include "Editor/EditorMgr.h"

using namespace Core;

const float32 LOADING_ANIM_DELAY = 0.1f; // delay (seconds) between two frames of animation
const float32 LOADING_ANIM_MIN_TIME = 0.5f; // minimum duration time of the animation being shown

Core::LoadingScreen::LoadingScreen():
	mAnimationTimer(false),
	mAnimationEndTimer(false)
{
	mAnimationFrame = 0;
	GfxSystem::GfxViewport viewport(Vector2_Zero, Vector2(1, 1), false, false);
	mRenderTarget = gGfxRenderer.AddRenderTarget(viewport, EntitySystem::EntityHandle::Null);
}

Core::LoadingScreen::~LoadingScreen()
{
	gGfxRenderer.RemoveRenderTarget(mRenderTarget);
}

void Core::LoadingScreen::DoLoading( eType type, const string& sceneName )
{
	mType = type;
	Init();

	// set up resource groups
	switch (mType)
	{
	case TYPE_BASIC_RESOURCES:

		// resources for the loading screen
		gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "general", "Loading", ".+Loading.\\.png");
		gResourceMgr.LoadResourcesInGroup("Loading");

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

		// load them into memory
		gResourceMgr.LoadResourcesInGroup("Scripts");

		// start up the GUI stuff
		gGUIMgr.Init();

		break;

	case TYPE_EDITOR:
		gEditorMgr.LoadEditor();
		break;

	case TYPE_SCENE:
		OC_ASSERT_MSG(!sceneName.empty(), "Undefined scene name");
		gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Project", sceneName));
		break;
	}


	// enforce minimum anim time limit
	while (mAnimationEndTimer.GetMilliseconds() < 1000.0f * LOADING_ANIM_MIN_TIME)
	{
		Draw();
	}

	Clean();
}

void Core::LoadingScreen::Init()
{
	if (mType != TYPE_BASIC_RESOURCES)
	{
		gResourceMgr.SetLoadingListener(this);
		mAnimationTimer.Reset();
		mAnimationEndTimer.Reset();
	}
}

void Core::LoadingScreen::Clean()
{
	gResourceMgr.SetLoadingListener(0);
}

void Core::LoadingScreen::Draw()
{
	if (mType == TYPE_BASIC_RESOURCES) // can't draw when we don't have the gui ready
		return;

	if (gGfxRenderer.BeginRendering())
	{
		gGfxRenderer.ClearScreen(GfxSystem::Color(30,30,30));
		gGfxRenderer.SetCurrentRenderTarget(mRenderTarget);
		if (mAnimationTimer.GetMilliseconds() >= 1000.0f * LOADING_ANIM_DELAY)
		{
			mAnimationFrame = (mAnimationFrame+1) % 8;
			mAnimationTimer.Reset();
		}
		GfxSystem::TexturePtr texRes = gResourceMgr.GetResource("Loading", "Loading" + StringConverter::ToString(mAnimationFrame) + ".png");
		if (texRes)
		{
			GfxSystem::TexturedQuad quad;
			quad.position.x = 0.5f;
			quad.position.y = 0.5f;
			quad.texture = texRes->GetTexture();
			quad.size.x = 200.0f;
			quad.size.y = 200.0f;
			gGfxRenderer.DrawTexturedQuad(quad);
		}
		gGfxRenderer.EndRendering();
	}
}

void Core::LoadingScreen::ResourceGroupLoadStarted( const string& groupName, uint32 resourceCount )
{
	OC_UNUSED(groupName);
	OC_UNUSED(resourceCount);
	Draw();
}

void Core::LoadingScreen::ResourceLoadStarted( const ResourceSystem::Resource* resource )
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
