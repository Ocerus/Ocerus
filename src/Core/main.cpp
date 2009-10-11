#include "Common.h"
#include "Application.h"

#include <exception>

#ifdef __WIN__
#include <Windows.h>

INT WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, 
					int nCmdShow )
{	
	try
	{
		Core::Application* app = new Core::Application();
		app->Init();
		app->RunMainLoop();
		delete app;
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

#else

int main(int argc, char* argv[])
{	
	try
	{
		Core::Application* app = new Core::Application();
		app->Init();
		app->RunMainLoop();
		delete app;
	}
	catch (std::exception& e)
	{
		gLogMgr.LogMessage("An exception has occured: ", e.what(), LOG_ERROR);
		fprintf(stderr, "An exception has occured!");
		return 1;
	}
	catch (...)
	{
		gLogMgr.LogMessage("An unknown exception has occured.", LOG_ERROR);
		fprintf(stderr, "An unknown exception has occured!");
		return 1;
	}
	return 0;
};

#endif

