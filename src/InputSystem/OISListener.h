#ifndef OISListener_h__
#define OISListener_h__

#include <OISMouse.h>
#include <OISKeyboard.h>
#include "../Utility/Settings.h"

namespace InputSystem
{

	/// @name Forward declarations.
	//@{
	class InputMgr;
	enum eKeyCode;
	//@}

	/** OIS library specific wrapper to hide its implementation.
	*/
	class OISListener : public OIS::MouseListener, public OIS::KeyListener
	{
	public:
		OISListener();
		virtual ~OISListener();

		/// @name MouseListener
		//@{
		virtual bool mouseMoved(const OIS::MouseEvent &evt);
		virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID);
		virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID);
		//@}

		/// @name KeyListener
		//@{
		virtual bool keyPressed(const OIS::KeyEvent &evt);
		virtual bool keyReleased(const OIS::KeyEvent &evt);
		//@}

		/// @name Process OIS events.
		void CaptureInput();

		/// @name True if the specified key is down.
		bool IsKeyDown(const eKeyCode k) const;

		/// @name Returns current state of the mouse.
		void GetMouseState(MouseState& state) const;

		/// @name Sets resolution of the screen.
		void SetResolution(uint32 width, uint32 height);

	private:
		InputMgr *mMgr;

		OIS::InputManager* mOIS;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;

		/// @name Conversion function.
		eMouseButton OisToMbtn(OIS::MouseButtonID id);
	};
}

#endif // OISListener_h__