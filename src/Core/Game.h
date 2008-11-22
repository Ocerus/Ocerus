#ifndef Game_h__
#define Game_h__

#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"

class b2World;

namespace Core
{
	enum eGameState { GS_NORMAL=0 };

	/** This class holds all info related directly to the game and takes care about rendering, input and game logic.
	*/
	class Game : public StateMachine<eGameState>
	{
	public:
		Game();
		virtual ~Game();

		void Init();
		void Deinit();
		void Update(float32 delta);
		void Draw();

		b2World* GetPhysics(void) const { assert (mPhysics); return mPhysics; }

	private:
		b2World* mPhysics;
	};
}

#endif // Game_h__