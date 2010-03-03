/// @file
/// Declares base class for property editors.

#ifndef _ABSTRACTPROPERTYEDITOR_H_
#define _ABSTRACTPROPERTYEDITOR_H_

#include "Base.h"
#include "Utils/Properties/PropertyHolder.h"

namespace CEGUI
{
	class Window;
	class EventArgs;
}

namespace Editor
{
	/// The AbstractPropertyEditor class is a base class for property editors. It
	/// implements aspects that are common to all property editors, such as...
	class AbstractPropertyEditor
	{
	public:
		/// Constructs a AbstractPropertyEditor that manages given property.
		AbstractPropertyEditor(const PropertyHolder& property);

		virtual ~AbstractPropertyEditor();

		CEGUI::Window* CreateLabelWidget(CEGUI::Window* parent);

		void ResetUpdateFlag() { mWasUpdated = false; }

		bool WasUpdated() const { return mWasUpdated; }

		PropertyHolder GetProperty() const { return mProperty; }

		virtual CEGUI::Window* CreateEditorWidget(CEGUI::Window* parent) = 0;

		virtual void UpdateEditorWidget() { mWasUpdated = true; }

		virtual void SubmitEditorWidget() = 0;

		/// Returns the height of this editor.
		virtual uint32 GetEditorHeight() const = 0;

	protected:
		/// Property managed by this editor.
		PropertyHolder mProperty;

	private:
		CEGUI::Window* mLabelWidget;
		bool mWasUpdated;
	};
}


#endif // _ABSTRACTPROPERTYEDITOR_H_