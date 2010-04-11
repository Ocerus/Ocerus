/// @file
/// An application state representing the actual game.

#ifndef Game_h__
#define Game_h__

#include "Base.h"
#include "StateMachine.h"
#include "InputSystem/IInputListener.h"

namespace Core
{
	/// Inner states of the game state.
	enum eGameState 
	{ 
		/// The game is loading.
		GS_INITING=0,
		/// The game is cleaning its resources.
		GS_CLEANING,
		/// Standard state of a running game.
		GS_NORMAL
	};

	/// This class holds all info related directly to the game itself and takes care about rendering, input and game
	/// logic. Basically it connects more parts of the whole system together to create the game engine. As the application
	/// itself it is state-driven as well which means the game can be only in a single state (Core::eGameState) at the
	/// given point of time.
	class Game : public StateMachine<eGameState>, public InputSystem::IInputListener
	{
	public:

		/// Default constructor.
		Game(void);

		/// Default destructor.
		virtual ~Game(void);


		/// @name Game control
		//@{

		/// Sets the target where the game will be rendered to.
		void SetRenderTarget(const GfxSystem::RenderTargetID renderTarget);

		/// Prepares game data structures and loads all necessary resources.
		void Init(void);

		/// Cleans up everything after the game is finished.
		void Clean(void);

		/// Updates the physics.
		void Update(const float32 delta);

		/// Renders the game.
		void Draw(const float32 delta);

		/// State of the game action.
		enum eActionState { AS_RUNNING, AS_PAUSED };

		/// Returns true if the game action is currently running.
		inline bool IsActionRunning(void) const { return GetActionState() == AS_RUNNING; }

		/// Returns the current action state.
		inline eActionState GetActionState(void) const { return mActionState; }

		/// Pauses the game action until resumed again.
		void PauseAction(void);

		/// Resumes the game action.
		void ResumeAction(void);

		/// Restarts the game action to the initial (saved) state.
		void RestartAction(void);

		/// Returns the current game action.
		inline uint64 GetTimeMillis(void) { return mTimer.GetMilliseconds(); }

		//@}


		/// @name Gameplay related stuff
		//@{

		/// Returns the physics engine.
		inline b2World* GetPhysics(void) { return mPhysics; }

		//@}


		/// @name Callbacks from InputSystem::IInputListener
		//@{

		/// Called by the InputSystem when a key is pressed.
		virtual bool KeyPressed(const InputSystem::KeyInfo& ke);

		/// Called by the InputSystem when a key is released.
		virtual bool KeyReleased(const InputSystem::KeyInfo& ke);

		/// Called when the mouse moves. Cursor position and other info are passed via parameter.
		virtual bool MouseMoved(const InputSystem::MouseInfo& mi);

		/// Called when a mouse button is pressed. Cursor position and other info are passed via parameter.
		virtual bool MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		/// Called when a mouse button is released. Cursor position and other info are passed via parameter.
		virtual bool MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		//@}


	private:

		// Game control.
		void UpdateGameProperties(void); ///< Updates globally accessible properties related to game.
		eActionState mActionState; ///< Is the game running or what?
		Utils::Timer mTimer; ///< Timer for game action related things.
		GfxSystem::RenderTargetID mRenderTarget; ///< Where the game is to be rendered?
		EntitySystem::EntityHandle mCamera; ///< Camera used for rendering.


		// Physics.
		Physics* mPhysics;
		float32 mPhysicsResidualDelta; ///< Part of the timestep delta we didn't use for the physics update last Update.
		/// A structure for queuing events from the physics engine.
		struct PhysicsEvent: ClassAllocation<PhysicsEvent, ALLOCATION_POOLED>
		{
			// Note that here shouldn't be any pointer to a shape cos it can be destroyed during ProcessPhysicsEvent.
			EntitySystem::EntityHandle entity1;
			EntitySystem::EntityHandle entity2;
		};
		typedef vector<PhysicsEvent*> PhysicsEventList;
		PhysicsEventList mPhysicsEvents;
		void ProcessPhysicsEvent(const PhysicsEvent& evt);


	private:

		class PhysicsCallbacks;
		friend class PhysicsCallbacks;
		PhysicsCallbacks* mPhysicsCallbacks;
		PhysicsDraw* mPhysicsDraw;
	};
}

#endif // Game_h__