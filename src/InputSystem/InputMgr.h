#ifndef InputMgr_h__
#define InputMgr_h__

#include <vector>
#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "InputActions.h"

/// Macro for easier use.
#define gInputMgr InputSystem::InputMgr::GetSingleton()

namespace OIS
{
	/// Forward declarations.
	//@{
	class Mouse;
	class Keyboard;
	//@}
}

namespace InputSystem
{
	/// Forward declarations.
	//@{
	class IInputListener;
	class OISListener;
	//@}

	/** This class processes all input from external devices such as mouse, keyboard or joystick. You can query its
		current state or register for event callbacks. Note that it must be updated reguralry by calling CaptureInput.
	*/
	class InputMgr : public Singleton<InputMgr>
	{
	public:
		InputMgr(void);
		~InputMgr(void);

		/// Updates the state of the manager and processes all events.
		void CaptureInput(void);
		
		/// Returns true if a specified key is down.
		bool IsKeyDown(eKeyCode k);

		/// Registers/unregisters event listeners.
		//@{
		void AddInputListener(IInputListener* listener);
		void RemoveInputListener(IInputListener* listener);
		void RemoveAllInputListeners(void);
		//@}

		/// Sets current resolution of the screen. To be called from the gfx system.
		void _SetResolution(uint32 width, uint32 height);
	private:
		/// OIS specific stuff
		//@{
		friend class OISListener;
		OISListener* mOISListener;
		//@}

		/// Collection of input event listeners.
		//@{
		typedef std::vector<IInputListener*> ListenersList;
		ListenersList mListeners;
		//@}
	};
}

#endif // InputMgr_h__