/// @file
/// Implementation of the InputSystem using OIS.

#ifndef OISListener_h__
#define OISListener_h__

#include "Base.h"

// We can safely include these headers as this file is included only by the InputSystem cpp files.
#include <OISMouse.h>
#include <OISKeyboard.h>

namespace InputSystem
{
	/// OIS library specific wrapper to hide its implementation.
	class OISListener : public OIS::MouseListener, public OIS::KeyListener
	{
	public:

		OISListener();
		virtual ~OISListener();

		/// @name Callbacks from OIS::MouseListener
		//@{
		virtual bool mouseMoved(const OIS::MouseEvent &evt);
		virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID);
		virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID);
		//@}

		/// @name Callbacks from OIS::KeyListener
		//@{
		virtual bool keyPressed(const OIS::KeyEvent &evt);
		virtual bool keyReleased(const OIS::KeyEvent &evt);
		//@}

		/// Processes OIS events.
		void CaptureInput(void);

		/// True if the specified key is down.
		bool IsKeyDown(const eKeyCode k) const;

		/// Returns the current state of the mouse.
		void GetMouseState(MouseState& state) const;

		/// Sets new resolution of the screen.
		/// You should make sure the resolution is always correct to prevent any problems with mouse.
		void SetResolution(uint32 width, uint32 height);

	private:

		InputMgr *mMgr;

		OIS::InputManager* mOIS;
		OIS::Mouse* mMouse;
		OIS::Keyboard* mKeyboard;

		eMouseButton OisToMbtn(OIS::MouseButtonID id);
	};
}

#endif // OISListener_h__