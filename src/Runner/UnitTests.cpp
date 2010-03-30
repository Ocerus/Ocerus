/// @file
/// Unit tests entry point.

#include "Common.h"
#include "UnitTests.h"
#include "LogSystem/LogMgr.h"
#include "Editor/LayerMgr.h"


int main(int argc, char* argv[])
{
	OC_UNUSED(argc);
	OC_UNUSED(argv);

	// init essential subsystems
	LogSystem::LogMgr::CreateSingleton();
	LogSystem::LogMgr::GetSingleton().Init("CoreLog.txt");


	// run tests
	UnitTest::RunAllTests();

	// wait for results
#ifdef __WIN__
	getchar();
#endif

	return 0;
}

void Test::InitResources( void )
{
	ResourceSystem::ResourceMgr::CreateSingleton();
	gResourceMgr.Init("test");
	gResourceMgr.AddResourceDirToGroup(".", "Test");
}

void Test::InitEntities( void )
{
	EntitySystem::EntityMgr::CreateSingleton();
}

void Test::InitEntities( const string& resourceFileName )
{
	InitEntities();
	OC_ASSERT(ResourceSystem::ResourceMgr::SingletonExists());
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Test", "entities.xml"));
}

void Test::CleanSubsystems( void )
{
	if (ResourceSystem::ResourceMgr::SingletonExists()) ResourceSystem::ResourceMgr::DestroySingleton();
	if (EntitySystem::EntityMgr::SingletonExists()) EntitySystem::EntityMgr::DestroySingleton();
	if (Editor::LayerMgr::SingletonExists()) Editor::LayerMgr::DestroySingleton();
}