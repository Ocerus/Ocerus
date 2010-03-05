/// @file
/// Declares an interface for value editors.

#ifndef _IVALUEEDITOR_H_
#define _IVALUEEDITOR_H_

#include "Base.h"

namespace CEGUI
{
	class Window;
}

namespace Editor
{
	/// The IValueEditor class declares an interface for value editors and also
	/// implements the basic functionality of update locking that can be used by
	/// subclasses.
	/// A value editor is an object that provides a mechanism for editing an
	/// arbitrary variable in the game. Every value editor consists of at least
	/// two widgets: label widget and editor widget. You usually create a value
	/// editor by constructing concrete subclass or by using a factory method
	/// and then you can create those widgets with CreateLabelWidget() and CreateEditorWidget().
	class IValueEditor
	{
	public:
		IValueEditor(): mUpdatesLocked(false) {}

		/// Creates the label widget and returns it. Caller must set its position
		/// and dimensions afterward. It's recommended to use GetEditorHeight()
		/// for the height.of the widget.
		virtual CEGUI::Window* CreateLabelWidget(CEGUI::Window* parent) = 0;

		/// Creates the editor widget and returns it. Caller must set its position
		/// and dimensions afterward. It's recommended to use GetEditorHeight()
		/// for the height.of the widget.
		virtual CEGUI::Window* CreateEditorWidget(CEGUI::Window* parent) = 0;

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		/// @note Subclass implementer is responsible for checking whether editor is
		/// locked for updates.
		virtual void Update() = 0;

		/// Submits the value from editor widget to the model.
		virtual void Submit() = 0;

		/// Returns the recommended height for editor widget.
		virtual uint32 GetEditorHeightHint() const = 0;

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