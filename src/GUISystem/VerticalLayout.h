/// @file
/// Position the GUI elements in a vertical layout and automatically repositions them when one of them changes its size.

#ifndef _VERTICALLAYOUT_H_
#define _VERTICALLAYOUT_H_

#include "Base.h"

namespace CEGUI
{
	class Window;
	class EventArgs;
}

namespace GUISystem
{

	/// The VerticalLayout class helps to keep GUI elements positioned in a vertical layout and automatically repositions
	/// them when one of them changes its size.
	/// Lifetime of this object is managed by the managed window, so you should always create it with new operator and
	/// should not delete it yourself.
	class VerticalLayout
	{
	public:
		/// Constructs a VerticalLayout that will manage child windows of managedWindow. The managed window must
		/// be empty during the construction.
		/// @param managedWindow Window managed by this object. All its children will be positioned.
		/// @param contentPane Some complex windows have child windows that are not supposed to be positioned (e.g.
		/// 		FrameWindow has TitleBar, etc.). It these cases you need to pass managed window's content pane.
		/// @param resizeParent Whether managed window should be also resized to minimal height necessary to properly
		/// 		display all children windows.
		VerticalLayout(CEGUI::Window* managedWindow, const CEGUI::Window* contentPane = 0, bool resizeParent = false);

		/// Destroys the VerticalLayout.
		~VerticalLayout();
		
		/// Adds child window to managed window. You must use this method (instead of managed window's addChildWindow()
		/// method).
		void AddChildWindow(CEGUI::Window* window);

		/// Returns the spacing between child windows in pixels.
		int GetSpacing() const { return mSpacing; }

		/// Sets the spacing between child windows in pixels.
		void SetSpacing(int spacing) { mSpacing = spacing; }
		
		/// Reposition the child elements (and resize managed window if resizeParent set), unless updates are locked.
		void UpdateLayout();

		/// Returns whether updates are locked. Updates are unlocked by default.
		bool LockedUpdates() const { return mLockedUpdates; }

		/// Lock updates. UpdateLayout() does nothing until updates are unlocked again.
		void LockUpdates() { mLockedUpdates = true; }

		/// Unlock updates. UpdateLayout() updates layout again until updates are locked again.
		void UnlockUpdates() { mLockedUpdates = false; }

		/// @name CEGUI Callbacks
		//@{
		bool OnManagedWindowDestructionStarted(const CEGUI::EventArgs&);
		bool OnChildWindowSized(const CEGUI::EventArgs&) { if (LockedUpdates()) return false; UpdateLayout(); return true; }
		bool OnChildWindowDestructionStarted(const CEGUI::EventArgs&);
		//@}

	private:
		VerticalLayout(const VerticalLayout&);
		VerticalLayout& operator=(const VerticalLayout&);

		typedef vector<CEGUI::Window*> WindowList;
		
		CEGUI::Window* mManagedWindow;
		const CEGUI::Window* mContentPane;
		WindowList mChildWindows;
		bool mResizeParent;
		int mSpacing;
		bool mLockedUpdates;
	};
}

#endif // _VERTICALLAYOUT_H_
