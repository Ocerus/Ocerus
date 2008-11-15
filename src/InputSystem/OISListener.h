#ifndef OISListener_h__
#define OISListener_h__

#include <OISMouse.h>
#include <OISKeyboard.h>
#include "../Utility/Settings.h"

namespace InputSystem
{

	/// Forward declarations.
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

		/// MouseListener
		//@{
		virtual bool mouseMoved(const OIS::MouseEvent &evt);
		virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID);
		virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID);
		//@}

		/// KeyListener
		//@{
		virtual bool keyPressed(const OIS::KeyEvent &evt);
		virtual bool keyReleased(const OIS::KeyEvent &evt);
		//@}

		/// Process OIS events.
		void CaptureInput();
		/// True if the specified key is down.
		bool IsKeyDown(eKeyCode k) const;

		/// Sets resolution of the screen.
		void SetResolution(uint32 width, uint32 height);

	private:
		InputMgr *mMgr;

		OIS::InputManager* mOIS;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;

		/// Conversion function.
		eMouseButton OisToMbtn(OIS::MouseButtonID id);
	};
}

#endif // OISListener_h__