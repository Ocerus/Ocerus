#include <Windows.h>
#include "Application.h"

INT WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, 
					int nCmdShow )
{	
	Core::Application* app = new Core::Application();
	app->runMainLoop();
	delete app;
	return 0;
};