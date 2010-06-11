/// @file
/// Manages key shortcuts for editor.

#ifndef KEY_SHORTCUTS_H
#define KEY_SHORTCUTS_H

#include "Base.h"

namespace Editor
{

	/// The ResourceWindow class manages GUI widget for listing resources.
	class KeyShortcuts
	{
	public:

		enum eKeyShortcut 
		{
			KS_TOOL_MOVE,
			KS_TOOL_ROTATE,
			KS_TOOL_ROTATE_Y,
			KS_TOOL_SCALE,
			KS_UKNOWN
		};

		/// Constructs a Shortcuts.
		KeyShortcuts();

		/// Destroys the Shortcuts.
		~KeyShortcuts();

		/// Shortcuts
		eKeyShortcut GetPressedShortcut(InputSystem::eKeyCode keyCode);

	private:

		enum ModKeys
		{
			MK_NOMOD =	0,
			MK_ALT =	(1 << 0),
			MK_CTRL =	(1 << 1),
			MK_SHIFT =	(1 << 2)
		};

		struct Shortcut
		{
			Shortcut(eKeyShortcut _shortcutEnum, uint8 _modKeyFlags):
				shortcutEnum(_shortcutEnum), modKeyFlags(_modKeyFlags) {}

			eKeyShortcut shortcutEnum;
			uint8 modKeyFlags;
		};

		typedef multimap<InputSystem::eKeyCode, Shortcut> ShortcutMap;

		ShortcutMap mShortcuts;

	};
}

#endif // KEY_SHORTCUTS_H
