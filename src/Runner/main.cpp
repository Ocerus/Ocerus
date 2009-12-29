/// @file
/// Application entry point.

struct AfterExitHook { ~AfterExitHook(void); };
AfterExitHook gAfterExitHook;

#include "../Setup/Settings.h"
#include "../Core/Application.h"
#include "../LogSystem/LogMgr.h"
#include "../LogSystem/LogMacros.h"
#include "../Memory/GlobalAllocation.h"
#include <exception>

#include <boost/regex.hpp>
#include "Utils/StringKey.h"

#ifdef __WIN__
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#endif

#undef USE_DBGLIB

#ifdef USE_DBGLIB
#include <DbgLib/DbgLib.h>
#endif


#if defined(USE_LEAKDETECTOR) && defined(NDEBUG)
#error Cannot use the leak detector in non debug builds!
#endif


#if defined(USE_DBGLIB)
/// Callback function from the DbgLib.
void DbgLibExceptionCallback(const DbgLib::tstring& dumpFilePath, void* params)
{
	stringstream ss;
	ss << "A system exception has occured!" << std::endl << std::endl;
	ss << "The MiniDump was written to" << std::endl;
	ss << dumpFilePath << std::endl << std::endl;
	ss << "Stack trace is stored in the MiniDump file.";

	string message = ss.str();

	#ifdef __WIN__
	MessageBox(NULL, message.c_str(), "A Windows exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	#else
	fprintf(stderr, "%s\n",  message.c_str());
	#endif

	// terminate the application, this exception is not fixable
	exit(1);
}
#endif


#if defined(USE_DBGLIB) && defined(USE_LEAKDETECTOR)
DbgLib::CMemLeakDetector* gLeakDetector = 0;
#endif


#ifdef __WIN__
INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
int main(int argc, char* argv[])
#endif // __WIN__
{	
	// initialize memory
	Memory::InitGlobalMemoryAllocation();

	// register debugging support using DbgLib
	#ifdef USE_DBGLIB

		// enable exception handlers
		DbgLib::CDebugFx::SetExceptionHandler(true);
		
		// install an exception callback
		DbgLib::CDebugFx::SetExceptionCallback(DbgLibExceptionCallback, NULL);

		#ifdef USE_LEAKDETECTOR
		// enable memory leak detection
		gLeakDetector = DbgLib::CDebugFx::GetMemLeakDetector();
		// optional: here we can set a custom reported by calling SetReporter()
		gLeakDetector->Enable();
		#endif

	#endif


	try
	{
		// run the application itself
		Core::Application* app = new Core::Application();
		app->Init();
		app->RunMainLoop();
		delete app;
	}
	catch (std::exception& e)
	{
		if (LogSystem::LogMgr::GetSingletonPtr())
		{
			LogSystem::LogMgr::GetSingleton().LogMessage(string("An exception has occured: ") + e.what(), LL_ERROR);
		}
		OC_FAIL(e.what());
		return -1;
	}
	catch (...)
	{
		if (LogSystem::LogMgr::GetSingletonPtr())
		{
			LogSystem::LogMgr::GetSingleton().LogMessage("An unknown exception has occured.", LL_ERROR);
		}
		OC_FAIL("An unknown exception has occured!");
		return -1;
	}

	return 0;
};


/// Callback function called after all static and global variables were destroyed.
AfterExitHook::~AfterExitHook()
{
	#if defined(USE_DBGLIB) && defined(USE_LEAKDETECTOR)
		gLeakDetector->Disable();
		gLeakDetector->ReportLeaks();
		// the instance should be automatically deallocated by DbgLib
	#endif
}
