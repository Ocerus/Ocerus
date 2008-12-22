#ifndef Game_h__
#define Game_h__

#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"
#include "../InputSystem/IInputListener.h"
#include "../EntitySystem/EntityMgr/EntityHandle.h"
#include <vector>

/// @name Forward declaration.
class b2World;

namespace Core
{
	/// @name Inner states of the game state.
	enum eGameState { GS_NORMAL=0 };

	/** This class holds all info related directly to the game and takes care about rendering, input and game logic.
	*/
	class Game : public StateMachine<eGameState>, public InputSystem::IInputListener
	{
	public:
		Game(void);
		virtual ~Game(void);

		void Init(void);
		void Deinit(void);
		void Update(const float32 delta);
		void Draw(const float32 delta);

		/// @name Returns a pointer to an object representing the physics system.
		b2World* GetPhysics(void) const { assert (mPhysics); return mPhysics; }

		/// @name Called when a keyboard key is pressed/released.
		//@{
		virtual void KeyPressed(const InputSystem::KeyInfo& ke);
		virtual void KeyReleased(const InputSystem::KeyInfo& ke);
		//@}

		/// @name Called when the mouse moves. Cursor position and other info is passed via parameter.
		virtual void MouseMoved(const InputSystem::MouseInfo& mi);
		/// @name Called when a mouse button is pressed.
		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		/// @name Called when a mouse button is released.
		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		/// @name Returns the size of the circle used for setting engine power.
		int32 GetEnginePowerCircleRadius(void) const;

	private:
		/// @name This object represents the physics engine.
		b2World* mPhysics;
		/// @name Part of the timestep delta we didn't use for the physics update last Update.
		float32 mPhysicsResidualDelta;

		/// @name Selections stuff.
		//@{
		EntitySystem::EntityHandle mHoveredEntity;
		typedef std::vector<EntitySystem::EntityHandle> EntityList;
		EntityList mSelectedEntities;
		//@}

		/// @name Camera stuff.
		EntitySystem::EntityHandle mCameraFocus;
	};
}

#endif // Game_h__