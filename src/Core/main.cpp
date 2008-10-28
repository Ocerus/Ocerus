#include <Windows.h>
#include "Application.h"
#include <exception>
#include "../Utility/Settings.h"

INT WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, 
					int nCmdShow )
{	
	try
	{
		Core::Application* app = DYN_NEW Core::Application();
		app->runMainLoop();
		DYN_DELETE app;
	}
	catch (std::exception& e)
	{
		//TODO write into log
		MessageBox(NULL,  e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		return 1;
	}
	return 0;
};