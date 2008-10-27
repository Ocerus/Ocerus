#include <Windows.h>
#include "Application.h"
#include <exception>
#include <iostream>

INT WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, 
					int nCmdShow )
{	
	try
	{
		Core::Application* app = new Core::Application();
		app->runMainLoop();
		delete app;
	}
	catch (std::exception& e)
	{
		//TODO write into log
		//TODO zobrazit konzoli, at to je videt i za behu
		std::cout << e.what();
		return 1;
	}
	return 0;
};