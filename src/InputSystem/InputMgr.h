#ifndef InputMgr_h__
#define InputMgr_h__

#include "Singleton.h"
#include "Settings.h"
#include "InputActions.h"
#include "../GfxSystem/IScreenListener.h"

/// @name Macro for easier use.
#define gInputMgr InputSystem::InputMgr::GetSingleton()

namespace OIS
{
	/// @name Forward declarations.
	//@{
	class Mouse;
	class Keyboard;
	//@}
}

namespace InputSystem
{
	/// @name Forward declarations.
	//@{
	class IInputListener;
	class OISListener;
	//@}

	/** This class processes all input from external devices such as mouse, keyboard or joystick. You can query its
		current state or register for event callbacks. Note that it must be updated reguralry by calling CaptureInput.
	*/
	class InputMgr : public Singleton<InputMgr>, public GfxSystem::IScreenListener
	{
	public:
		InputMgr(void);
		~InputMgr(void);

		/// @name Updates the state of the manager and processes all events.
		void CaptureInput(void);
		
		/// @name Returns true if a specified key is down.
		bool IsKeyDown(const eKeyCode k) const;

		/// @name Returns true if a specified button of the mouse is pressed.
		bool IsMouseButtonPressed(const eMouseButton btn) const;

		/// @name Returns current state of the mouse.
		MouseState& GetMouseState(void) const;

		/// @name Registers/unregisters event listeners.
		//@{
		void AddInputListener(IInputListener* listener);
		void RemoveInputListener(IInputListener* listener);
		void RemoveAllInputListeners(void);
		//@}

		/// @name Callback from the GfxSystem.
		virtual void ResolutionChanged(int x, int y);

	private:
		/// @name OIS specific stuff
		//@{
		friend class OISListener;
		OISListener* mOISListener;
		//@}

		/// @name Mouse state cache.
		mutable MouseState mMouseStateCache;

		/// @name Collection of input event listeners.
		//@{
		typedef vector<IInputListener*> ListenersList;
		ListenersList mListeners;
		//@}
	};
}

#endif // InputMgr_h__