/// @file
/// Application entry point.

#include "../Setup/Settings.h"
#include "../Core/Application.h"
#include "../LogSystem/LogMgr.h"
#include "../LogSystem/LogMacros.h"
#include <exception>

#ifdef __WIN__
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#endif


//#undef USE_DBGLIB		//Muhe: dbglib nefunguje v express VS

#ifdef USE_DBGLIB
#include <DbgLib/DbgLib.h>
#endif



#if defined(USE_LEAKDETECTOR) && defined(NDEBUG)
#error Cannot use the leak detector in non debug builds!
#endif


#ifdef USE_DBGLIB
/// Callback function from the DbgLib.
void DbgLibExceptionCallback(void* params)
{
	const char* message = "A system exception has occured!\nTODO how to retrieve any detailed info?";
	#ifdef __WIN__
	MessageBox(NULL, message, "A Windows exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	#else
	fprintf(stderr, "%s\n",  message);
	#endif

	// terminate the application, this exception is not fixable
	exit(1);
}
#endif


#ifdef __WIN__
INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
int main(int argc, char* argv[])
#endif // __WIN__
{	
	// register debugging support using DbgLib
	#ifdef USE_DBGLIB

		// enable exception handlers
		DbgLib::CDebugFx::SetExceptionHandler(true);
		
		// install an exception callback
		DbgLib::CDebugFx::SetExceptionCallback(DbgLibExceptionCallback, NULL);

		#ifdef USE_LEAKDETECTOR
		// enable memory leak detection
		DbgLib::CMemLeakDetector* leakDetector = DbgLib::CDebugFx::GetMemLeakDetector();
		// optional: here we can set a custom reported by calling SetReporter()
		leakDetector->Enable();
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
		LogSystem::LogMgr::GetSingleton().LogMessage(string("An exception has occured: ") + e.what(), LL_ERROR);
		#ifdef __WIN__
		MessageBox(NULL,  e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
		return -1;
	}
	catch (...)
	{
		LogSystem::LogMgr::GetSingleton().LogMessage("An unknown exception has occured.", LL_ERROR);
		#ifdef __WIN__
		MessageBox(NULL, "An unknown exception has occured!", "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
		return -1;
	}

	// unregister DbgLib
	#ifdef USE_DBGLIB
		#ifdef USE_LEAKDETECTOR
		leakDetector->Disable();
		leakDetector->ReportLeaks();
		#endif
	#endif

	return 0;
};

