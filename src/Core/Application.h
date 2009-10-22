/// @file
/// Application entry point.

#ifndef APPLICATION_h__
#define APPLICATION_h__

#include "Base.h"
#include "Singleton.h"
#include "StateMachine.h"
#include "Timer.h"

/// Macro for easier use.
#define gApp Core::Application::GetSingleton()

/// Main namespace of the application.
namespace Core
{
	/// State which the application can be in.
	enum eAppState 
	{ 
		/// The application is initializing basic data.
		AS_INITING=0, 
		/// The application is loading resources needed to enter the fist state.
		AS_LOADING, 
		/// A game is currently in the progress.
		AS_GAME, 
		/// The application is shutting down.
		AS_SHUTDOWN
	};

	/// Main class of the whole application. One instance is created at startup and the RunMainLoop() method is invoked.
	class Application : public StateMachine<eAppState>, public Singleton<Application>
	{
	public:

		/// Does all necessary init prior to the main execution.
		Application(void);

		/// Cleans up everything.
		virtual ~Application(void);

		/// Inits the application (creates singletons, ...)
		void Init(void);

		/// Main loop of the whole project.
		void RunMainLoop(void);

		/// Shuts the application down and cleans everything.
		void Shutdown(void);

		/// Resets FPS counters and other stats measured in the main loop.
		void ResetStats(void);
		
		/// Returns the average FPS.
		inline float32 GetAvgFPS(void) const { return mAvgFPS; }

		/// Returns FPS measured in the last frame.
		inline float32 GetLastFPS(void) const { return mLastFPS; }

		/// Current application time.
		inline uint64 GetCurrentTimeMillis(void) { return mTimer.GetMilliseconds(); }

		/// Displays the debug console.
		void ShowConsole(void);

		/// Writes text into the debug console.
		void WriteToConsole(const string& str);

		/// Hides the debug console.
		void HideConsole(void);

	private:

		/// Application states.
		LoadingScreen* mLoadingScreen;
		Game* mGame;
		
		/// Represents global settings of the application.
		Config* mGlobalConfig;

		/// Helper method for processing window events.	
		void MessagePump(void);

		/// Stuff for measuring performance and time.
		typedef deque<uint64> TimesList;
		TimesList mFrameDeltaTimes;
		Timer mTimer;
		/// During this time (in seconds) the frame delta time will be averaged
		float32 mFrameSmoothingTime;
		float32 CalculateFrameDeltaTime(void);

		/// Performance stats
		uint64 mLastSecond;
		uint64 mFrameCount;
		float32 mLastFPS;
		float32 mAvgFPS;
		void UpdateStats();

		/// Debug console stuff.
		uint32 mConsoleHandle;
		int32 mConsoleX;
		int32 mConsoleY;
		int32 mConsoleWidth;
		int32 mConsoleHeight;

	};
}

#endif 