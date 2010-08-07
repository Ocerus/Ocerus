/// @file
/// Declares an abstract value editor with model.

#ifndef _ABSTRACTVALUEEDITOR_H_
#define _ABSTRACTVALUEEDITOR_H_

#include "Base.h"
#include "Models/IValueEditorModel.h"
#include "GUISystem/CEGUIForwards.h"
namespace CEGUI
{
	class String;
	class Window;
	class Editbox;
	class PushButton;
}

namespace Editor
{
	/// The AbstractValueEditor class declares an interface for value editors and also
	/// implements the basic functionality of update locking that can be used by
	/// subclasses. Moreover, it provides methods that can be used in subclasses.
	class AbstractValueEditor
	{
	public:
		AbstractValueEditor(): mEditorWidget(0), mUpdatesLocked(false) {}

		virtual ~AbstractValueEditor();

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

		/// Returns whether editor is locked for updates.
		bool UpdatesLocked() const { return mUpdatesLocked; }

	protected:
		/// Lock editor for updates. Update() will not update editor until it is unlocked again.
		void LockUpdates() { mUpdatesLocked = true; }

		/// Unlock editor for updates. Update() will update editor as long as it stays unlocked.
		void UnlockUpdates() { mUpdatesLocked = false; }

		/// Creates a new static text widget, sets the common properties and returns it.
		CEGUI::Window* CreateStaticTextWidget(const CEGUI::String& name, const CEGUI::String& text, const CEGUI::String& tooltip);

		/// Creates the main editor label widget and returns it.
		CEGUI::Window* CreateEditorLabelWidget(const CEGUI::String& name, const Editor::IValueEditorModel* model);

		/// Creates a button for removing elements and returns it.
		CEGUI::PushButton* CreateRemoveElementButtonWidget(const CEGUI::String& name);

		/// Returns the recommended height of an editbox.
		static float GetEditboxHeight();

		/// Adds widget to tab navigation.
		void AddWidgetToTabNavigation(CEGUI::Window* widget);

	protected:
		/// Main widget of the editor. The widget is automatically destroyed in AbstractValueEditor's destructor.
		CEGUI::Window* mEditorWidget;
		
	private:
		bool mUpdatesLocked;
	};
}

#endif // _ABSTRACTVALUEEDITOR_H_