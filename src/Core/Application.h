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
/// This is the main part of the whole system. It contains its entry point as well as other classes related to the 
/// application itself. The main class of the system is Application representing the only single instance of the 
/// running application and provides information about its state. The application is designed to behave like a state
/// machine because for games it's typical to be in a single state (loading, menu, game, ...). Each of these states
/// are represented by a class inside Core.
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
	/// The application is state-driven which means that it can be in a single state (Core::eAppState) at a given point of
	/// time. Each of these states are represented by a class inside Core.
	class Application : public StateMachine<eAppState>, public Singleton<Application>
	{
	public:

		/// Does all necessary init prior to the main execution.
		Application(void);

		/// Cleans up everything.
		virtual ~Application(void);

		/// Inits the application (creates singletons, ...).
		void Init();

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

		/// Registers the game state for receiving the input.
		void RegisterGameInputListener(InputSystem::IInputListener* listener);

		/// Unregisters the game state for receiving the input.
		void UnregisterGameInputListener(InputSystem::IInputListener* listener);

		/// Returns whether the editor support is turned on.
		bool IsDevelopMode() const { return mDevelopMode; }

		/// Returns whether the editor is currently turned on and the game is running only in a small window.
		bool IsEditMode() const { return mEditMode; }
		
		/// Returns true if the application has currently focus.
		bool HasFocus(void) const { return mHasFocus; }

		/// Returns project used for the game itself.
		Project* GetGameProject() { return mGameProject; }

		/// Opens a PDF document.
		void OpenPDF(const string& filePath);

		/// Gives time to run to other applications for a while.
		void YieldProcess();

		/// Fills the vector with platforms where the game can be deployed.
		void GetAvailableDeployPlatforms(vector<string>& out);

		/// Deploys the current project into the specified directory.
		void DeployCurrentProject(const string& platform, const string& destination);

	private:

		/// Application states.
		LoadingScreen* mLoadingScreen;
		Game* mGame;

		/// Application settings.
		bool mDevelopMode; ///< if true the editor support is turned on.
		bool mEditMode; ///< if true the editor is currently turned on and the game is running only in a small window.
		Project* mGameProject; ///< Project used for the game itself.
		bool mHasFocus;

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