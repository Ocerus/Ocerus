/// @file
/// Declares an abstract value editor with model.

#ifndef _EDITOR_ABSTRACTVALUEEDITOR_H_
#define _EDITOR_ABSTRACTVALUEEDITOR_H_

#include "Base.h"
#include "ValueEditorTypes.h"
#include "GUISystem/CEGUIForwards.h"

namespace Editor
{
	/// The AbstractValueEditor class declares an interface for value editors and also
	/// implements the basic functionality of update locking that can be used by
	/// subclasses. Moreover, it provides methods that can be used in subclasses.
	class AbstractValueEditor
	{
	public:
		/// Constructs an AbstractValueEditor.
		AbstractValueEditor(): mEditorWidget(0), mUpdatesLocked(false) {}

		/// Destroys the AbstractValueEditor.
		virtual ~AbstractValueEditor() {}

		/// Returns the widget of the editor.
		CEGUI::Window* GetWidget() { return mEditorWidget; }

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		/// @note Subclass implementer is responsible for checking whether editor is
		/// locked for updates.
		virtual void Update() = 0;

		/// Submits the value from editor widget to the model.
		virtual void Submit() = 0;

		/// Returns the type of value editor.
		virtual eValueEditorType GetType() = 0;

		/// Destroys the model.
		virtual void DestroyModel() = 0;

		/// Resets the widget so the editor can be stored to editors cache. 
		virtual void ResetWidget();

		/// Returns whether editor is locked for updates.
		bool UpdatesLocked() const { return mUpdatesLocked; }

	protected:
		/// Lock editor for updates. Update() will not update editor until it is unlocked again.
		void LockUpdates() { mUpdatesLocked = true; }

		/// Unlock editor for updates. Update() will update editor as long as it stays unlocked.
		void UnlockUpdates() { mUpdatesLocked = false; }

		/// Creates a label widget with specified name and returns it.
		CEGUI::Window* CreateLabelWidget(const CEGUI::String& name);
		
		/// Creates a button for removing elements with specified name and returns it.
		CEGUI::Window* CreateRemoveButtonWidget(const CEGUI::String& name);

		/// Creates a checkbox for setting IsShared status with specified name and returns it.
		CEGUI::Checkbox* CreateIsSharedCheckboxWidget(const CEGUI::String& name);

		/// Creates an image widget with a lock with specified name and returns it.
		CEGUI::Window* CreateIsLockedImageWidget(const CEGUI::String& name);

		/// Returns the recommended height of an editbox.
		static float GetEditboxHeight();

		/// Adds widget to tab navigation.
		void AddWidgetToTabNavigation(CEGUI::Window* widget);

		/// Returns ValueEditorFactory.
		ValueEditorFactory* GetValueEditorFactory();

	protected:
		/// Main widget of the editor.
		CEGUI::Window* mEditorWidget;

	private:
		bool mUpdatesLocked;
	};
}

#endif // _EDITOR_ABSTRACTVALUEEDITOR_H_