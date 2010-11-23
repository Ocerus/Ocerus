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
		/// The game is not loaded.
		GS_NOT_INITED=0,
		/// The game is loading.
		GS_INITING,
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

		/// Constructs the game while defining a directory to store temporary game stuff.
		Game(const string& tempDirectory);

		/// Default destructor.
		virtual ~Game(void);


		/// @name Game control
		//@{

		/// Name of the entity with the game camera.
		static const string GameCameraName;

		/// Refresh the game camera;
		void CreateDefaultRenderTarget();
		
		/// Sets the target where the game will be rendered to.
		void SetRenderTarget(const GfxSystem::RenderTargetID renderTarget);
		
		/// Gets the root window for in-game GUI elements.
		CEGUI::Window* GetRootWindow() { return mRootWindow; }
		
		/// Sets the root window for in-game GUI elements.
		void SetRootWindow(CEGUI::Window* window);
		
		/// Create the default root window.
		void CreateDefaultRootWindow();

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

		/// Save the game state to which the restart action will rollback.
		void SaveAction(void);
		
		/// Restarts the game action to the initial (saved) state.
		void RestartAction(void);

		/// Returns the current game time.
		inline uint64 GetTimeMillis(void) { return mTimer.GetMilliseconds(); }

		/// Saves the information about the game to a XML stream.
		bool SaveGameInfoToStorage(ResourceSystem::XMLOutput& storage);

		/// Load the information about the game from a XML stream.
		bool LoadGameInfoFromResource(ResourceSystem::ResourcePtr res);

		//@}


		/// @name Gameplay related stuff
		//@{

		/// Returns the physics engine.
		inline Physics* GetPhysics(void) { return mPhysics; }

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
		
		/// @name Dynamic properties
		//@{
		
		/// Clears the dynamic property list.
		inline void ClearDynamicProperties() { mDynamicProperties.ClearProperties(); }
		
		/// Returns whether the specified dynamic property exists.
		inline bool HasDynamicProperty(const string& propName) const { return mDynamicProperties.HasProperty(propName); }
		
		/// Deletes the specified dynamic property.
		inline bool DeleteDynamicProperty(const string& propName) { return mDynamicProperties.DeleteProperty(propName); }
		
		/// Gets the specified dynamic property.
		template<typename T>
		T GetDynamicProperty(const string& propName) const
		{
		  AbstractProperty* prop = mDynamicProperties.GetProperty(propName);
		  if (prop != 0 && prop->GetType() == Reflection::PropertyTypes::GetTypeID<T>()) return prop->GetValue<T>(0);
		  else return Reflection::PropertyTypes::GetDefaultValue<T>();
		}
		
		/// Sets the specified dynamic property.
		template<typename T>
		void SetDynamicProperty(const string& propName, const T& value)
		{
		  AbstractProperty* prop = mDynamicProperties.GetProperty(propName);
		  if (prop != 0)
		  {
		    if (prop->GetType() == Reflection::PropertyTypes::GetTypeID<T>()) prop->SetValue<T>(0, value);
		  }
		  else
		  {
		    prop = new ValuedProperty<T>(propName, PA_FULL_ACCESS, "");
			  if (mDynamicProperties.AddProperty(prop)) 
			  {
			    PropertySystem::GetProperties()->push_back(prop);
			    prop->SetValue<T>(0, value);
			  }
			  else delete prop;
		  }
		}
		
		/// Saves the dynamic properties to a XML stream.
		bool SaveDynamicPropertiesToStorage(ResourceSystem::XMLOutput& storage);
		
		/// Load the dynamic properties from a XML stream.
		bool LoadDynamicPropertiesFromResource(ResourceSystem::ResourcePtr res);
		
		//@}
		
		/// Name of directory to which the files with the game state are saved.
		static const char* SavePath;
		
		/// Saves the game to the specified file.
		bool SaveToFile(const string& fileName);
		
		/// Loads the game from the specified file.
		bool LoadFromFile(const string& fileName);

	private:

		// Game control.
		void UpdateGameProperties(void); ///< Updates globally accessible properties related to game.
		eActionState mActionState; ///< Is the game running?
		bool mActionRestarted; ///< True if the action was restarted before it was run again.
		Utils::Timer mTimer; ///< Timer for game action related things.
		GfxSystem::RenderTargetID mRenderTarget; ///< Where the game is to be rendered?
		EntitySystem::EntityHandle mCamera; ///< Camera used for rendering.
		CEGUI::Window* mRootWindow;  ///< Root window for in-game GUI elements.
		Reflection::PropertyMap mDynamicProperties; ///< Dymanic properties saved with the game accessible from scripts.
		string mTempActionSave; ///< File for temporary save of the action.


		// Physics.
		Physics* mPhysics;
		float32 mPhysicsResidualDelta; ///< Part of the timestep delta we didn't use for the physics update last Update.
		/// A structure for queuing events from the physics engine.
		struct PhysicsEvent: ClassAllocation<PhysicsEvent, ALLOCATION_POOLED>
		{
			// Note that here shouldn't be any pointer to a shape cos it can be destroyed during ProcessPhysicsEvent.
			enum eType { COLLISION_STARTED, COLLISION_ENDED, COLLISION_PRESOLVED };
			eType type;
			EntitySystem::EntityHandle entity1;
			EntitySystem::EntityHandle entity2;
			Vector2 normal; ///< From entity1 to entity2. World coordinates.
			Vector2 contactPoint; ///< World coordinates.
		};
		typedef vector<PhysicsEvent*> PhysicsEventList;
		PhysicsEventList mPhysicsEvents;
		void ProcessPhysicsEvent(const PhysicsEvent& evt);

	private:

		class PhysicsCallbacks;
		friend class PhysicsCallbacks;
		PhysicsCallbacks* mPhysicsCallbacks;
		GfxSystem::PhysicsDraw* mPhysicsDraw;
	};
}

#endif // Game_h__