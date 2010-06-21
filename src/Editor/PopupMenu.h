/// @file
/// Popup menus in the editor.

#ifndef PopupMenu_h__
#define PopupMenu_h__

#include "Base.h"
#include <CEGUIEvent.h>

namespace Editor
{
	/// Manages all popup menus in the editor.
	class PopupMenu
	{
	public:

		/// Creates the menu given its string ID.
		/// @param selfDestruct If true the menu object will be deleted after closed.
		PopupMenu(const string& menuName, const bool selfDestruct = false);

		/// Initializes the menu.
		/// @param data The data the menu operates on.
		inline void Init(Reflection::AbstractProperty* data);

		/// Initializes the menu.
		/// @param data The data the menu operates on.
		template<typename T>
		void Init(const T& data);

		/// Default destructor.
		~PopupMenu();

		/// Returns the data the menu operates on.
		template<typename T>
		T GetData() const;

		/// Opens the menu at the given screen position.
		void Open(float32 x, float32 y);

		/// Closes the menu.
		void Close();

	private:

		string mName;
		bool mSelfDestruct;
		bool mInited;
		Reflection::AbstractProperty* mData;
		list<CEGUI::Event::Connection> mEventConnections;

		void Init();
		void InitResourceTypes();
		void InitComponentTypes();
		void ConfigureMenu(CEGUI::Window* parent);

		bool OnMenuMouseUp(const CEGUI::EventArgs&);
		bool OnMenuItemMouseUp(const CEGUI::EventArgs&);

	};
}

//-----------------------------------------------------------------------------
// Implementation


void Editor::PopupMenu::Init( Reflection::AbstractProperty* data )
{
	mData = data;
	Init();
}

template<typename T>
void Editor::PopupMenu::Init( const T& data )
{
	mData = new Reflection::ValuedProperty<T>("PopupData", Reflection::PA_NONE, "");
	mData->SetValue<T>(0, data);
	Init();
}

template<typename T>
T Editor::PopupMenu::GetData() const
{
	OC_ASSERT(mData);
	return mData->GetValue<T>(0);
}


#endif // PopupMenu_h__