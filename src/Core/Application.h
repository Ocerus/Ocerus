#ifndef APPLICATION_h__
#define APPLICATION_h__

#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"
#include "../Utility/Timer.h"
#include "../Utility/Singleton.h"
#include <deque>

/// @name Macro for easier use.
#define gApp Core::Application::GetSingleton()

/// @name  Forward declarations
//@{
namespace ResourceSystem { class ResourceMgr; }
namespace InputSystem { class InputMgr; }
namespace GfxSystem { class GfxRenderer; }
namespace EntitySystem { class EntityMgr; }
namespace LogSystem { class LogMgr; }
namespace GUISystem { class GUIMgr; }
namespace StringSystem { class StringMgr; }
class Timer;
//@}

namespace Core
{

	/// @name  Forward declarations.
	//@{
	class LoadingScreen;
	class Game;
	class Config;
	//@}

	/// @name State which the application can be in.
	enum eAppState { AS_INITING=0, AS_LOADING, AS_GAME, AS_SHUTDOWN, AS_GUI };

	/** Main class of the whole project. One instance is created at startup and the runMainLoop method is invoked.
	*/
	class Application : public StateMachine<eAppState>, public Singleton<Application>
	{
	public:
		/// @name Does all necessary init prior to the main execution.
		Application(void);
		/// @name Cleans up everything.
		virtual ~Application(void);

		/// @name Inits the application (creates singletons, ...)
		void Init(void);

		/// @name Main loop of the whole project.
		void RunMainLoop(void);

		/// @name Shuts the application down and clean everything
		void Shutdown(void);

		/// @name Reset FPS counters and other stats measured in the main loop.
		void ResetStats(void);
		
		/// @name  Stats getters.
		//@{
		inline float32 GetAvgFPS(void) const { return mAvgFPS; }
		inline float32 GetLastFPS(void) const { return mLastFPS; }
		//@}

		/// @name  Console debug window stuff.
		//@{
		void ShowConsole(void);
		void WriteToConsole(const string& str);
		void HideConsole(void);
		//@}

		/// @name Provides access to the global config functionality.
		inline Config* GetGlobalConfig(void) const { return mGlobalConfig; }

		/// @name  State getters
		//@{
		Game* GetCurrentGame(void) const { assert(mGame); return mGame; }
		//@}

	private:
		/// @name  Singletons.
		//@{
		ResourceSystem::ResourceMgr* mResourceMgr;
		StringSystem::StringMgr* mStringMgr;
		InputSystem::InputMgr* mInputMgr;
		GfxSystem::GfxRenderer* mGfxRenderer;
		EntitySystem::EntityMgr* mEntityMgr;
		LogSystem::LogMgr* mLogMgr;
		GUISystem::GUIMgr* mGUIMgr;
		//@}

		/// @name  Application state screens.
		//@{
		LoadingScreen* mLoadingScreen;
		Game* mGame;
		//@}

		/// @name Represents global settings of the application. Available via getter.
		Config* mGlobalConfig;

		/// @name Helper method for processing window events.	
		void MessagePump(void);

		/// @name  Stuff for measuring performance and time.
		//@{
		typedef std::deque<uint64> TimesList;
		TimesList mFrameDeltaTimes;
		Timer mTimer;
		float32 mFrameSmoothingTime; // during this time (in seconds) the frame delta time will be averaged
		float32 CalculateFrameDeltaTime(void);
		//@}

		/// @name  Performance stats
		//@{
		uint64 mLastSecond;
		uint64 mFrameCount;
		float32 mLastFPS;
		float32 mAvgFPS;
		void UpdateStats();
		//@}

		/// @name  Debug console stuff.
		//@{
		uint32 mConsoleHandle;
		int32 mConsoleX;
		int32 mConsoleY;
		int32 mConsoleWidth;
		int32 mConsoleHeight;
		//@}

	};
}

#endif 