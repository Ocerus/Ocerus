/// @file
/// A GUI layout component.

#ifndef GUILayout_h__
#define GUILayout_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// A GUI layout with callbacks visible in the game.
	class GUILayout : public RTTIGlue<GUILayout, Component>
	{
	public:

		/// Called after the component is created.
		virtual void Create(void);

		/// Called before the component is destroyed.
		virtual void Destroy(void);

		/// Called when a new message arrives.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Called from RTTI when the component is allowed to set up its properties.
		static void RegisterReflection(void);

		/// Layout used for defining the GUI.
		ResourceSystem::ResourcePtr GetLayout(void) const { return mLayout; }

		/// Layout used for defining the GUI.
		void SetLayout(ResourceSystem::ResourcePtr value);
		
		/// Scheme used for defining the GUI.
		ResourceSystem::ResourcePtr GetScheme(void) const { return mScheme; }

		/// Scheme used for defining the GUI.
		void SetScheme(ResourceSystem::ResourcePtr value);
		
		/// Script module with functions used as a callback for GUI events.
		ResourceSystem::ResourcePtr GetCallback(void) const { return mCallback; }

		/// Script module with functions used as a callback for GUI events.
		void SetCallback(ResourceSystem::ResourcePtr value);
		
		/// Returns whether the GUI layout is visible.
		bool GetVisible(void) const { return mVisible; }

		/// Sets whether the GUI layout is visible.
		void SetVisible(bool value);

	private:

		ResourceSystem::ResourcePtr mLayout;
		ResourceSystem::ResourcePtr mScheme;
		ResourceSystem::ResourcePtr mCallback;
		bool mVisible;
		
		CEGUI::Window* mRootWindow;
		bool mScriptUpdateError;
		
    void ReloadWindow(void);
	};
}

#endif