/// @file
/// Unit tests entry point.

#include "Common.h"
#include "UnitTests.h"
#include "LogSystem/LogMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "StringSystem/StringMgr.h"


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
	gResourceMgr.Init("test/");
}

void Test::InitEntities( void )
{
	EntitySystem::EntityMgr::CreateSingleton();
}

void Test::InitEntities( const string& resourceFileName )
{
	InitEntities();
	OC_ASSERT(ResourceSystem::ResourceMgr::SingletonExists());
	gResourceMgr.AddResourceFileToGroup("entities.xml", "entities"); 
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("entities", "entities.xml"));
}

void Test::CleanSubsystems( void )
{
	if (ResourceSystem::ResourceMgr::SingletonExists()) ResourceSystem::ResourceMgr::DestroySingleton();
	if (EntitySystem::EntityMgr::SingletonExists()) EntitySystem::EntityMgr::DestroySingleton();
	if (EntitySystem::LayerMgr::SingletonExists()) EntitySystem::LayerMgr::DestroySingleton();
	if (StringSystem::StringMgr::SingletonExists()) StringSystem::StringMgr::DestroySingleton();
}