/// @file
/// Definitions of the CEGUI properties for CEGUI::ItemListbox.

#ifndef ItemListboxProperties_h__
#define ItemListboxProperties_h__

namespace GUISystem
{
	void ShowPopup(CEGUI::PopupMenu* popupMenu, const CEGUI::MouseEventArgs& e);

	/// Declaration of a custom mouse wheel property for ItemListbox.
	class ItemListboxWantsMouseWheel : public CEGUI::Property
	{
	public:

		/// Default constructor.
		inline ItemListboxWantsMouseWheel() : CEGUI::Property(
			"WantsMouseWheel",
			"If true the window will receive the mouse wheel events.",
			"True"),
			mValue(true)
		{}

		/// Default destructor.
		inline virtual ~ItemListboxWantsMouseWheel() {}

		inline virtual CEGUI::String get(const CEGUI::PropertyReceiver* receiver) const { return mValue ? "True" : "False"; }
		inline virtual void set(CEGUI::PropertyReceiver* receiver, const CEGUI::String& value) { mValue = StringConverter::FromString<bool>(value.c_str()); }

	private:

		bool mValue;
	};
}

#endif // ItemListboxProperties_h__