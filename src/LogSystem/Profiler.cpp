/// @file
/// Real-time interactive ingame profiling.

#include "Common.h"
#include "Profiler.h"
#include "../Core/Application.h"

#ifdef USE_PROFILER
#include <RTHProfiler.h>
#endif


LogSystem::Profiler::Profiler( void )
{
#ifdef USE_PROFILER
	RTHProfiler::CProfileManager::Reset();
	RTHProfiler::CProfileManager::Pause();
#endif
}

void LogSystem::Profiler::DumpIntoConsole( void )
{
#ifdef USE_PROFILER
	RTHProfiler::CProfileIterator* iter = RTHProfiler::CProfileManager::Get_Iterator();

	gApp.WriteToConsole("PROFILER DATA:\n");

	int32 indentLevel = 0;
	while (true)
	{
		bool done = false;

		while (iter->Is_Done())
		{
			if (indentLevel == 0)
			{
				done = true;
				break;
			}
			indentLevel--;
			iter->Enter_Parent();
			iter->Next();
		}

		if (done) break;

		stringstream ss;
		for (int32 i=0; i<indentLevel; ++i) ss << '\t';
		ss << iter->Get_Current_Name() << ": " << iter->Get_Current_Total_Calls() << "  " << iter->Get_Current_Total_Time() << '\n';
		gApp.WriteToConsole(ss.str());

		indentLevel++;
		iter->Enter_Current_Child();
	}

	RTHProfiler::CProfileManager::Release_Iterator(iter);
#endif
}

void LogSystem::Profiler::Start( void )
{
#ifdef USE_PROFILER
	ocInfo << "Profiling started";
	RTHProfiler::CProfileManager::Resume();
#endif
}

void LogSystem::Profiler::Stop( void )
{
#ifdef USE_PROFILER
	RTHProfiler::CProfileManager::Pause();
	ocInfo << "Profiling ended";
#endif
}

void LogSystem::Profiler::Update( void )
{
#ifdef USE_PROFILER
	RTHProfiler::CProfileManager::Update();
#endif
}

bool LogSystem::Profiler::IsRunning( void )
{
#ifdef USE_PROFILER
	return RTHProfiler::CProfileManager::Is_Enabled();
#else
	return false;
#endif
}