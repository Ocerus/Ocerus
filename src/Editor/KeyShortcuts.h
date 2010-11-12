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

		/// Representation of a shortcut definition.
		typedef uint32 KeyShortcut;

		static const KeyShortcut KS_TOOL_MOVE = 1<<1; ///< Entity movement tool toggle.
		static const KeyShortcut KS_TOOL_ROTATE = 1<<2; ///< Entity rotation tool toggle.
		static const KeyShortcut KS_TOOL_ROTATE_Y = 1<<3; ///< Entity Y rotation tool toggle.
		static const KeyShortcut KS_TOOL_SCALE = 1<<4; ///< Entity scaling tool toggle.

		static const KeyShortcut KS_START_ACTION	= 1<<5; ///< Action start.
		static const KeyShortcut KS_RESTART_ACTION  = 1<<6; ///< Action restart.
		static const KeyShortcut KS_PLAY_PAUSE		= 1<<7; ///< Action pause/unpause.

		static const KeyShortcut KS_PROFILER = 1<<8; ///< Profiler start/stop.
		
		static const KeyShortcut KS_CAM_LEFT = 1<<9; ///< Camera left movement.
		static const KeyShortcut KS_CAM_RIGHT = 1<<10; ///< Camera right movement.
		static const KeyShortcut KS_CAM_UP = 1<<11; ///< Camera up movement.
		static const KeyShortcut KS_CAM_DOWN = 1<<12; ///< Camera down movement.

		static const KeyShortcut KS_OPEN_PROJECT = 1<<14; ///< Open existing project.

		static const KeyShortcut KS_CREATE_SCENE = 1<<13; ///< New scene creation.
		static const KeyShortcut KS_SAVE_SCENE = 1<<16; ///< Save of the current scene.

		static const KeyShortcut KS_DUPLICATE = 1<<17; ///< Entity duplication.
		static const KeyShortcut KS_DELETE = 1<<18; ///< Entity deletion.
		
		static const KeyShortcut KS_QUIT = 1<<19; ///< Application quit.
		
		static const KeyShortcut KS_FULLSCREEN = 1<<20; ///< Fullscreen toggle.

		static const KeyShortcut KS_CENTER_CAM = 1<<21; ///< Camera centering on the current entity.
		
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
