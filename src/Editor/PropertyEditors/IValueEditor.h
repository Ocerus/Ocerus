/// @file
/// Declares an interface for value editors.

#ifndef _IVALUEEDITOR_H_
#define _IVALUEEDITOR_H_

#include "Base.h"

namespace CEGUI
{
	class String;
	class Window;
}

namespace Editor
{
	/// The IValueEditor class declares an interface for value editors and also
	/// implements the basic functionality of update locking that can be used by
	/// subclasses.
	/// A value editor is an object that provides a mechanism for editing an
	/// arbitrary variable in the game.
	class IValueEditor
	{
	public:
		IValueEditor(): mUpdatesLocked(false) {}

		/// Creates the editor widget and returns it. The value editor manages its
		/// height so you should not change it. Some value editors can even change
		/// their height during their lifetime (e.g expanding and collapsing the editor),
		/// so you might want to handle the EventSized event (or you can use a layout
		/// manager such as GUISystem::VerticalLayout).
		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix) = 0;

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		/// @note Subclass implementer is responsible for checking whether editor is
		/// locked for updates.
		virtual void Update() = 0;

		/// Submits the value from editor widget to the model.
		virtual void Submit() = 0;

	protected:
		/// Lock editor for updates. Update() will not update editor until it is unlocked again.
		void LockUpdates() { mUpdatesLocked = true; }

		/// Unlock editor for updates. Update() will update editor as long as it stays unlocked.
		void UnlockUpdates() { mUpdatesLocked = false; }

		/// Returns whether editor is locked for updates.
		bool UpdatesLocked() const { return mUpdatesLocked; }

	private:
		bool mUpdatesLocked;
	};
}

#endif // _IVALUEEDITOR_H_