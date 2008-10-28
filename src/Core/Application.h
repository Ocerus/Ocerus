#ifndef APPLICATION_h__
#define APPLICATION_h__

#include "../Common.h"
#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"
#include "../Utility/Timer.h"
#include "../Utility/Singleton.h"
#include <deque>

#define gApp Core::Application::GetSingleton()

namespace Core
{

	class LoadingScreen;
	class Game;

	enum eAppState { AS_INITING=0, AS_LOADING, AS_GAME, AS_SHUTDOWN };

	class Application : public StateMachine<eAppState>, public Singleton<Application>
	{
	public:
		Application(void);
		virtual ~Application(void);

		void runMainLoop(void);

		void ResetStats(void);
		inline float32 GetAvgFPS(void) { return mAvgFPS; }
		inline float32 GetLastFPS(void) { return mLastFPS; }

		// console debug window
		void ShowConsole(void);
		void WriteToConsole(const string& str);
		void HideConsole(void);
	private:
		// singletons
		ResourceSystem::ResourceMgr* mResourceMgr;
		InputSystem::InputMgr* mInputMgr;
		GfxSystem::GfxRenderer* mGfxRenderer;
		EntitySystem::EntityMgr* mEntityMgr;
		LogSystem::LogMgr* mLogMgr;

		LoadingScreen* mLoadingScreen;
		Game* mGame;

		void MessagePump(void);

		// frame delta times
		typedef std::deque<uint64> TimesList;
		TimesList mFrameDeltaTimes;
		Timer mTimer;
		float32 mFrameSmoothingTime; // during this time (in seconds) the frame delta time will be averaged
		float32 CalculateFrameDeltaTime(void);

		// performance stats
		uint64 mLastSecond;
		uint64 mFrameCount;
		float32 mLastFPS;
		float32 mAvgFPS;
		void UpdateStats();

	};
}

#endif 