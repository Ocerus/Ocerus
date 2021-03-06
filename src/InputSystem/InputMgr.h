/// @file
/// Entry point to the InputSystem.

#ifndef InputMgr_h__
#define InputMgr_h__

#include "Base.h"
#include "Singleton.h"
#include "../GfxSystem/IGfxWindowListener.h"

/// Macro for easier use.
#define gInputMgr InputSystem::InputMgr::GetSingleton()

/// Input system manages events from all input devices connected to the computer which are required by the game.
/// The subsystem interface is provided by InputMgr.
namespace InputSystem
{
	// The forward declaration here is allowed as this serves only for internal purposes of InputMgr and thus it doesn't
	// make sense to forward declare it in Forwards.h.
	class OISListener;

	/// This class processes all input from external devices such as mouse, keyboard or joystick. You can query its
	///	current state or register for event callbacks.
	/// Note that it must be updated regurarly by calling CaptureInput().
	class InputMgr : public Singleton<InputMgr>, public GfxSystem::IGfxWindowListener
	{
	public:

		/// Constructor.
		InputMgr(void);

		/// Destructor.
		~InputMgr(void);

		/// Updates the state of the manager and processes all events.
		void CaptureInput(void);

		/// Releases all keys and buttons.
		void ReleaseAll(void);

		/// Returns true if a specified key is down.
		bool IsKeyDown(const eKeyCode k) const;

		/// Returns true if a specified button of the mouse is pressed.
		bool IsMouseButtonPressed(const eMouseButton btn) const;

		/// Returns the current state of the mouse.
		MouseState& GetMouseState(void) const;


		/// @name Event listeners
		//@{

		/// Registers an event listener.
		void AddInputListener(IInputListener* listener);

		/// Registers an event listener which will receive the events before any other.
		void AddInputListenerToFront(IInputListener* listener);

		/// Unregisters an event listener.
		void RemoveInputListener(IInputListener* listener);

		/// Unregisters all event listeners.
		void RemoveAllInputListeners(void);

		//@}


		/// @name Callbacks from GfxSystem::IScreenListener
		//@{
		virtual void ResolutionChanged(const uint32 width, const uint32 height);
		//@}

	private:

		/// OIS specific stuff
		friend class OISListener;
		OISListener* mOISListener;

		/// Mouse state cache.
		mutable MouseState mMouseStateCache;

		///Collection of input event listeners.
		typedef vector<IInputListener*> ListenersList;
		ListenersList mListeners;

	};
}

#endif // InputMgr_h__