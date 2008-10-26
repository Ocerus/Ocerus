#ifndef Game_h__
#define Game_h__

#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"

namespace Core
{
	enum eGameState { GS_NORMAL=0 };

	class Game : public StateMachine<eGameState>
	{
	public:
		Game();
		virtual ~Game();

		void Init();
		void Deinit();
		void Update(float32 delta);
		void Draw();
	};
}

#endif // Game_h__