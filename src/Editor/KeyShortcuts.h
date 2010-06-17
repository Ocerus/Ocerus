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

		typedef uint32 KeyShortcut;
		static const KeyShortcut KS_TOOL_MOVE = 1<<1;
		static const KeyShortcut KS_TOOL_ROTATE = 1<<2;
		static const KeyShortcut KS_TOOL_ROTATE_Y = 1<<3;
		static const KeyShortcut KS_TOOL_SCALE = 1<<4;
		static const KeyShortcut KS_START_ACTION = 1<<5;
		static const KeyShortcut KS_RESTART_ACTION = 1<<6;
		static const KeyShortcut KS_PROFILER = 1<<7;
		
		/// Constructs a Shortcuts.
		KeyShortcuts();

		/// Destroys the Shortcuts.
		~KeyShortcuts();

		/// Sets the flags at the shortcuts belonging to the given key code.
		void KeyPressed(InputSystem::eKeyCode keyCode);

		/// Returns true if a shortcut is currently activ.
		bool IsShortcutActive(KeyShortcut shortcut);

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
			Shortcut(KeyShortcut _shortcutEnum, uint8 _modKeyFlags):
				shortcutEnum(_shortcutEnum), modKeyFlags(_modKeyFlags) {}

			KeyShortcut shortcutEnum;
			uint8 modKeyFlags;
		};

		typedef multimap<InputSystem::eKeyCode, Shortcut> ShortcutMap;

		ShortcutMap mShortcuts;
		KeyShortcut mShortcutMask;

	};
}

#endif // KEY_SHORTCUTS_H
