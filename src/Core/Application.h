#ifndef APPLICATION_h__
#define APPLICATION_h__

#include "../Common.h"
#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"
#include "../Utility/Timer.h"
#include "../Utility/Singleton.h"
#include "Config.h"
#include <deque>

/// Macro for easier use.
#define gApp Core::Application::GetSingleton()

namespace Core
{

	/// Forward declarations.
	//@{
	class LoadingScreen;
	class Game;
	//@}

	/// State which the application can be in.
	enum eAppState { AS_INITING=0, AS_LOADING, AS_GAME, AS_SHUTDOWN, AS_GUI };

	/** Main class of the whole project. One instance is created at startup and the runMainLoop method is invoked.
	*/
	class Application : public StateMachine<eAppState>, public Singleton<Application>
	{
	public:
		/// Does all necessary init prior to the main execution.
		Application(void);
		/// Cleans up everything.
		virtual ~Application(void);

		/// Main loop of the whole project.
		void runMainLoop(void);

		/// Reset FPS counters and other stats measured in the main loop.
		void ResetStats(void);
		
		/// Stats getters.
		//@{
		inline float32 GetAvgFPS(void) { return mAvgFPS; }
		inline float32 GetLastFPS(void) { return mLastFPS; }
		//@}

		/// Console debug window stuff.
		//@{
		void ShowConsole(void);
		void WriteToConsole(const string& str);
		void HideConsole(void);
		//@}

		/// Provides access to the global config functionality.
		inline Config* GetGlobalConfig(void) { return mGlobalConfig; }

	private:
		/// Singletons.
		//@{
		ResourceSystem::ResourceMgr* mResourceMgr;
		InputSystem::InputMgr* mInputMgr;
		GfxSystem::GfxRenderer* mGfxRenderer;
		EntitySystem::EntityMgr* mEntityMgr;
		LogSystem::LogMgr* mLogMgr;
		GUISystem::GUIMgr* mGUIMgr;
		//@}

		/// Application state screens.
		//@{
		LoadingScreen* mLoadingScreen;
		Game* mGame;
		//@}

		/// Represents global settings of the application. Available via getter.
		Config* mGlobalConfig;

		/// Helper method for processing window events.	
		void MessagePump(void);

		/// Stuff for measuring performance and time.
		//@{
		typedef std::deque<uint64> TimesList;
		TimesList mFrameDeltaTimes;
		Timer mTimer;
		float32 mFrameSmoothingTime; // during this time (in seconds) the frame delta time will be averaged
		float32 CalculateFrameDeltaTime(void);
		//@}

		/// Performance stats
		//@{
		uint64 mLastSecond;
		uint64 mFrameCount;
		float32 mLastFPS;
		float32 mAvgFPS;
		void UpdateStats();
		//@}

		/// Debug console stuff.
		//@{
		HWND mConsoleHandle;
		int32 mConsoleX;
		int32 mConsoleY;
		int32 mConsoleWidth;
		int32 mConsoleHeight;
		//@}

	};
}

#endif 