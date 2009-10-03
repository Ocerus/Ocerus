#include "Common.h"
#include "Application.h"

#include <Windows.h>
#include <exception>

/** Application entry point.
*/
INT WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, 
					int nCmdShow )
{	
	try
	{
		Core::Application* app = DYN_NEW Core::Application();
		app->Init();
		app->RunMainLoop();
		DYN_DELETE app;
	}
	catch (std::exception& e)
	{
		gLogMgr.LogMessage("An exception has occured: ", e.what(), LOG_ERROR);
		MessageBox(NULL,  e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		return 1;
	}
	catch (...)
	{
		gLogMgr.LogMessage("An unknown exception has occured.", LOG_ERROR);
		MessageBox(NULL, "An unknown exception has occured!", "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		return 1;
	}
	return 0;
};