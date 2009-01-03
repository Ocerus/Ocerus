#ifndef Game_h__
#define Game_h__

#include "../Utility/StateMachine.h"
#include "../Utility/Settings.h"
#include "../InputSystem/IInputListener.h"
#include "../EntitySystem/EntityMgr/EntityHandle.h"
#include "../EntitySystem/EntityMgr/EntityEnums.h"
#include "Dynamics/b2WorldCallbacks.h"
#include <vector>

/// @name Forward declarations.
//@{
class b2World;
class b2Shape;
class hgeDistortionMesh;
//@}

namespace Core
{
	/// @name Inner states of the game state.
	enum eGameState { GS_NORMAL=0 };

	/** This class holds all info related directly to the game and takes care about rendering, input and game logic.
	*/
	class Game : public StateMachine<eGameState>, public InputSystem::IInputListener, public b2ContactFilter, public b2ContactListener
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

		/// @name Input callbacks.
		//@{
		virtual void KeyPressed(const InputSystem::KeyInfo& ke);
		virtual void KeyReleased(const InputSystem::KeyInfo& ke);
		/// @name Called when the mouse moves. Cursor position and other info is passed via parameter.
		virtual void MouseMoved(const InputSystem::MouseInfo& mi);
		/// @name Called when a mouse button is pressed.
		virtual void MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		/// @name Called when a mouse button is released.
		virtual void MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);
		//@}

		/// @name Returns the size of the circle used for setting engine power.
		int32 GetEnginePowerCircleRadius(void) const;

		/// @name Returns the size of the circle used for setting weapon angle.
		int32 GetWeaponCircleRadius(void) const;

		/// @name Physics callbacks.
		//@{
		virtual bool ShouldCollide(b2Shape* shape1, b2Shape* shape2);
		virtual void Add(const b2ContactPoint* point);
		//@}

	private:
		/// @name This object represents the physics engine.
		b2World* mPhysics;
		/// @name Part of the timestep delta we didn't use for the physics update last Update.
		float32 mPhysicsResidualDelta;
		/// @name Structures for queuing evens from the physics engine.
		//@{
		struct PhysicsEvent
		{
			// Note that here shouldn't be any pointer to a shape cos it can be destroyed during ProcessPhysicsEvent.
			EntitySystem::EntityHandle entity1;
			EntitySystem::EntityHandle entity2;
		};
		typedef std::vector<PhysicsEvent*> PhysicsEventList;
		PhysicsEventList mPhysicsEvents;
		void ProcessPhysicsEvent(const PhysicsEvent& evt);
		//@}

		/// @name Water stuff.
		//@{
		hgeDistortionMesh* mWaterDistMesh;
		int32 mWaterDistRows;
		int32 mWaterDistCols;
		int32 mWaterDistCellW;
		int32 mWaterDistCellH;
		//@}

		/// @name Selections stuff.
		//@{
		EntitySystem::EntityHandle mHoveredEntity;
		typedef std::vector<EntitySystem::EntityHandle> EntityList;
		EntityList mSelectedEntities;
		#define MAX_SELECTED_GROUPS 10
		EntityList mSelectedGroups[MAX_SELECTED_GROUPS];
		//@}

		/// @name Player stuff.
		//@{
		EntitySystem::TeamID mMyTeam;
		//@}

		/// @name Camera stuff.
		//@{
		EntitySystem::EntityHandle mCameraFocus;
		Vector2 mCameraGrabWorldPos;
		//@}
	};
}

#endif // Game_h__