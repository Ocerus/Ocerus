#include "Common.h"
#include "KeyShortcuts.h"

using namespace Editor;


KeyShortcuts::KeyShortcuts()
{
	mShortcutMask = 0;

	typedef pair<InputSystem::eKeyCode, Shortcut> ShortcutPair;

	// defining shorcuts

	// editor tools
	mShortcuts.insert(ShortcutPair(InputSystem::KC_Q, Shortcut(KS_TOOL_MOVE, MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_W, Shortcut(KS_TOOL_ROTATE, MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_E, Shortcut(KS_TOOL_ROTATE_Y, MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_R, Shortcut(KS_TOOL_SCALE, MK_NOMOD)));
	
	// start/pause/stop
	mShortcuts.insert(ShortcutPair(InputSystem::KC_F9, Shortcut(KS_START_ACTION, MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_F9, Shortcut(KS_RESTART_ACTION, MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_SPACE, Shortcut(KS_PLAY_PAUSE, MK_CTRL)));
	
	// profiler
	mShortcuts.insert(ShortcutPair(InputSystem::KC_F5, Shortcut(KS_PROFILER, MK_CTRL)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_O, Shortcut(KS_OPEN_PROJECT, MK_CTRL)));

	// scene shortcuts
	mShortcuts.insert(ShortcutPair(InputSystem::KC_N, Shortcut(KS_CREATE_SCENE, MK_CTRL)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_S, Shortcut(KS_SAVE_SCENE, MK_CTRL)));

	// entity shortcuts
	mShortcuts.insert(ShortcutPair(InputSystem::KC_C, Shortcut(KS_DUPLICATE, MK_CTRL)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_DELETE, Shortcut(KS_DELETE, MK_NOMOD)));

	// Quit
	mShortcuts.insert(ShortcutPair(InputSystem::KC_ESCAPE, Shortcut(KS_QUIT, MK_NOMOD)));

	// Fullscreen
	mShortcuts.insert(ShortcutPair(InputSystem::KC_RETURN, Shortcut(KS_FULLSCREEN, MK_ALT)));


	// shortcuts which need to be held down to make effect

	// camera movement
	mHeldShortcuts.insert(ShortcutPair(InputSystem::KC_LEFT, Shortcut(KS_CAM_LEFT, MK_NOMOD)));
	mHeldShortcuts.insert(ShortcutPair(InputSystem::KC_RIGHT, Shortcut(KS_CAM_RIGHT, MK_NOMOD)));
	mHeldShortcuts.insert(ShortcutPair(InputSystem::KC_UP, Shortcut(KS_CAM_UP, MK_NOMOD)));
	mHeldShortcuts.insert(ShortcutPair(InputSystem::KC_DOWN, Shortcut(KS_CAM_DOWN, MK_NOMOD)));
}

KeyShortcuts::~KeyShortcuts()
{

}

uint8 Editor::KeyShortcuts::GetModFlags()
{
	uint8 modKeyFlags = MK_NOMOD;
	// checking if alt is pressed
	if (gInputMgr.IsKeyDown(InputSystem::KC_LMENU) || gInputMgr.IsKeyDown(InputSystem::KC_RMENU)) modKeyFlags |= MK_ALT;

	// checking if ctrl is pressed
	if (gInputMgr.IsKeyDown(InputSystem::KC_LCONTROL) || gInputMgr.IsKeyDown(InputSystem::KC_RCONTROL)) modKeyFlags |= MK_CTRL;

	// checking if shift is pressed
	if (gInputMgr.IsKeyDown(InputSystem::KC_LSHIFT) || gInputMgr.IsKeyDown(InputSystem::KC_RSHIFT)) modKeyFlags |= MK_SHIFT;

	return modKeyFlags;
}

void Editor::KeyShortcuts::KeyPressed( InputSystem::eKeyCode keyCode )
{
	mShortcutMask = 0;

	uint8 modKeyFlags = GetModFlags();

	pair<ShortcutMap::iterator, ShortcutMap::iterator> ret = mShortcuts.equal_range(keyCode);

	ShortcutMap::iterator it;
	for (it=ret.first; it!=ret.second; ++it)
	{
		if (it->second.modKeyFlags == modKeyFlags) mShortcutMask |= it->second.shortcutEnum;
	}
}

void Editor::KeyShortcuts::ShortcutsHeld()
{
	mShortcutMask = 0;
	uint8 modKeyFlags = GetModFlags();

	ShortcutMap::iterator it;
	for (it = mHeldShortcuts.begin(); it!=mHeldShortcuts.end(); ++it)
	{
		if (gInputMgr.IsKeyDown(it->first))
		{
			if (it->second.modKeyFlags == modKeyFlags) mShortcutMask |= it->second.shortcutEnum;
		}
	}
}

bool Editor::KeyShortcuts::IsShortcutActive( KeyShortcut shortcut )
{
	return (mShortcutMask & shortcut) != 0;
}