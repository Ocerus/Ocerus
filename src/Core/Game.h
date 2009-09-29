/// @file
/// An application state representing the actual game.

#ifndef Game_h__
#define Game_h__

#include "StateMachine.h"
#include "Settings.h"
#include "ResourcePointers.h"
#include "../InputSystem/IInputListener.h"
#include "../EntitySystem/EntityMgr/EntityHandle.h"
#include "../EntitySystem/EntityMgr/EntityEnums.h"
#include "Dynamics/b2WorldCallbacks.h"

class b2World;
class b2Shape;

namespace Core
{
	/// Inner states of the game state.
	enum eGameState 
	{ 
		GS_NORMAL=0 
	};

	class WaterSurface;

	/// This class holds all info related directly to the game itself and takes care about rendering, input and game logic.
	class Game : public StateMachine<eGameState>, public InputSystem::IInputListener, public b2ContactFilter, public b2ContactListener
	{
	public:

		Game(void);
		virtual ~Game(void);


		/// @name Game control
		//@{

		/// Prepares game data structures and loads all necessary resources.
		void Init(void);

		/// Cleans up everything after the game is finished.
		void Deinit(void);

		/// Updates the physics.
		void Update(const float32 delta);

		/// Renders the game.
		void Draw(const float32 delta);

		//@}


		/// @name Gameplay related stuff
		//@{

		/// Returns the size of the circle used for setting the engine power.
		int32 GetEnginePowerCircleRadius(void) const;

		/// Returns the size of the circle used for setting the weapon angle.
		int32 GetWeaponCircleRadius(void) const;

		/// Returns the physics engine.
		inline b2World* GetPhysics(void) { return mPhysics; }

		//@}


		/// @name Callbacks from InputSystem::IInputListener
		//@{

		/// Called by the InputSystem when a key is pressed.
		virtual void KeyPressed(const InputSystem::KeyInfo& ke);

		/// Called by the InputSystem when a key is released.
		virtual void KeyReleased(const InputSystem::KeyInfo& ke);

		/// Called when the mouse moves. Cursor position and other info are passed via parameter.
		virtual void MouseMoved(const InputSystem::MouseInfo& mi);

		/// Called when a mouse button is pressed. Cursor position and other info are passed via parameter.
		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		/// Called when a mouse button is released. Cursor position and other info are passed via parameter.
		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		//@}


		/// @name Callbacks from b2ContactFilter and b2ContactListener
		//@{
		virtual bool ShouldCollide(b2Shape* shape1, b2Shape* shape2);
		virtual void Add(const b2ContactPoint* point);
		//@}

	private:

		/// This object represents the physics engine.
		b2World* mPhysics;
		/// Part of the timestep delta we didn't use for the physics update last Update.
		float32 mPhysicsResidualDelta;

		/// A structure for queuing events from the physics engine.
		struct PhysicsEvent
		{
			// Note that here shouldn't be any pointer to a shape cos it can be destroyed during ProcessPhysicsEvent.
			EntitySystem::EntityHandle entity1;
			EntitySystem::EntityHandle entity2;
		};
		typedef vector<PhysicsEvent*> PhysicsEventList;
		PhysicsEventList mPhysicsEvents;
		void ProcessPhysicsEvent(const PhysicsEvent& evt);

		/// Water stuff.
		WaterSurface* mWaterSurface;
		vector<GfxSystem::ParticleSystemPtr> mBubbleEffects;

		/// Selections stuff.
		uint64 mLastClickTime;
		EntitySystem::EntityHandle mHoveredEntity;
		typedef vector<EntitySystem::EntityHandle> EntityList;
		EntityList mSelectedEntities;
		#define MAX_SELECTED_GROUPS 10
		EntityList mSelectedGroups[MAX_SELECTED_GROUPS];

		/// Player stuff.
		EntitySystem::TeamID mMyTeam;

		/// Camera stuff.
		EntitySystem::EntityHandle mCameraFocus;
		Vector2 mCameraGrabWorldPos;
	};
}

#endif // Game_h__