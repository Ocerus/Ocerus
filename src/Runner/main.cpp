/// @file
/// Application entry point.

#include "../Core/Application.h"
#include "../LogSystem/LogMgr.h"
#include "../LogSystem/LogMacros.h"
#include <exception>


#ifdef __WIN__
#include <Windows.h>
INT WINAPI WinMain (HINSTANCE hInstance, 
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, 
					int nCmdShow )
#else
int main(int argc, char* argv[])
#endif // __WIN__
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

	return 0;
};

