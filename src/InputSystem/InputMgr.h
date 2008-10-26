#ifndef InputMgr_h__
#define InputMgr_h__

#include <vector>
#include "../Utility/Singleton.h"
#include "../Utility/Settings.h"
#include "InputActions.h"

#define gInputMgr InputSystem::InputMgr::GetSingleton()

namespace OIS
{
	class Mouse;
	class Keyboard;
}

namespace InputSystem
{
	class IInputListener;
	class OISListener;

	class InputMgr : public Singleton<InputMgr>
	{
	public:
		InputMgr(void);
		~InputMgr(void);

		inline void CaptureInput(void);
		inline bool IsKeyDown(eKeyCode k);

		void AddInputListener(IInputListener* listener);
		void RemoveInputListener(IInputListener* listener);
		void RemoveAllInputListeners(void);

		inline void _SetResolution(uint32 width, uint32 height);
	private:
		friend class OISListener;
		typedef std::vector<IInputListener*> ListenersList;
		ListenersList mListeners;

		OISListener* mOISListener;
	};
}

#endif // InputMgr_h__