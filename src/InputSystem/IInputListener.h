#ifndef IInputListener_h__
#define IInputListener_h__

#include "InputActions.h"

namespace InputSystem
{
	class IInputListener
	{
	public:
		IInputListener(void);
		virtual ~IInputListener(void);

		virtual void KeyPressed(const KeyInfo& ke) = 0;
		virtual void KeyReleased(const KeyInfo& ke) = 0;

		virtual void MouseMoved(const MouseInfo& mi) = 0;
		virtual void MouseButtonPressed(const eMouseButton btn) = 0;
		virtual void MouseButtonReleased(const eMouseButton btn) = 0;

	};
}

#endif // IInputListener_h__