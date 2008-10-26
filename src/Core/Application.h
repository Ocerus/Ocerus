#ifndef APPLICATION_h__
#define APPLICATION_h__

#include "../Common.h"
#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"
#include "../Utility/Timer.h"
#include <deque>

namespace Core
{

	class LoadingScreen;
	class Game;

	enum eAppState { AS_INITING=0, AS_LOADING, AS_GAME, AS_SHUTDOWN };

	class Application : public StateMachine<eAppState>
	{
	public:
		Application(void);
		virtual ~Application(void);

		void runMainLoop(void);
	private:
		// singletons
		ResourceSystem::ResourceMgr* mResourceMgr;
		InputSystem::InputMgr* mInputMgr;
		GfxSystem::GfxRenderer* mGfxRenderer;
		EntitySystem::EntityMgr* mEntityMgr;

		LoadingScreen* mLoadingScreen;

		Game* mGame;

		// frame delta times
		typedef std::deque<uint64> TimesList;
		TimesList mFrameDeltaTimes;
		Timer mTimer;
		float32 mFrameSmoothingTime; // during this time (in seconds) the frame delta time will be averaged

		float32 CalculateFrameDeltaTime(void);
	};
}

#endif 