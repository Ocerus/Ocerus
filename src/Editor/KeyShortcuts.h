/// @file
/// Manages key shortcuts for editor.

#ifndef KEY_SHORTCUTS_H
#define KEY_SHORTCUTS_H

#include "Base.h"

namespace Editor
{

	/// The KeyShortcut class manages key shortcuts.
	class KeyShortcuts
	{
	public:

		typedef uint32 KeyShortcut;
		static const KeyShortcut KS_TOOL_MOVE = 1<<1;
		static const KeyShortcut KS_TOOL_ROTATE = 1<<2;
		static const KeyShortcut KS_TOOL_ROTATE_Y = 1<<3;
		static const KeyShortcut KS_TOOL_SCALE = 1<<4;

		static const KeyShortcut KS_START_ACTION	= 1<<5;
		static const KeyShortcut KS_RESTART_ACTION  = 1<<6;
		static const KeyShortcut KS_PLAY_PAUSE		= 1<<7;

		static const KeyShortcut KS_PROFILER = 1<<8;
		
		static const KeyShortcut KS_CAM_LEFT = 1<<9;
		static const KeyShortcut KS_CAM_RIGHT = 1<<10;
		static const KeyShortcut KS_CAM_UP = 1<<11;
		static const KeyShortcut KS_CAM_DOWN = 1<<12;

		static const KeyShortcut KS_CREATE_PROJ = 1<<13;
		static const KeyShortcut KS_OPEN_PROJ = 1<<14;
		static const KeyShortcut KS_SAVE_PROJ = 1<<15;

		static const KeyShortcut KS_SAVE_SCENE = 1<<16;

		static const KeyShortcut KS_DUPLICATE = 1<<17;
		static const KeyShortcut KS_DELETE = 1<<18;
		
		static const KeyShortcut KS_QUIT = 1<<19;
		
		/// Constructs a Shortcuts.
		KeyShortcuts();

		/// Destroys the Shortcuts.
		~KeyShortcuts();

		/// Sets the flags at the shortcuts belonging to the given key code.
		void KeyPressed(InputSystem::eKeyCode keyCode);

		/// Sets the flags at the held shortcuts belonging to the given key code.
		void ShortcutsHeld();

		/// Returns true if a shortcut is currently activ.
		bool IsShortcutActive(KeyShortcut shortcut);

	private:

		/// Modifying keys flags
		enum ModKeys
		{
			MK_NOMOD =	0,
			MK_ALT =	(1 << 0),
			MK_CTRL =	(1 << 1),
			MK_SHIFT =	(1 << 2)
		};

		/// Returns the modifying keys flags
		uint8 GetModFlags();

		/// Holds info about modifying keys and assigned shortcut
		struct Shortcut
		{
			Shortcut(KeyShortcut _shortcutEnum, uint8 _modKeyFlags):
				shortcutEnum(_shortcutEnum), modKeyFlags(_modKeyFlags) {}

			KeyShortcut shortcutEnum;
			uint8 modKeyFlags;
		};

		/// Maps pressed key to shortcuts
		typedef multimap<InputSystem::eKeyCode, Shortcut> ShortcutMap;

		ShortcutMap mShortcuts;
		ShortcutMap mHeldShortcuts;

		KeyShortcut mShortcutMask;

	};
}

#endif // KEY_SHORTCUTS_H
