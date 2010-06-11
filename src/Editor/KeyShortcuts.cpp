#include "Common.h"
#include "KeyShortcuts.h"

using namespace Editor;


KeyShortcuts::KeyShortcuts()
{
	typedef pair<InputSystem::eKeyCode, Shortcut> ShortcutPair;

	// defining shorcuts
	mShortcuts.insert(ShortcutPair(InputSystem::KC_Q, Shortcut(KS_TOOL_MOVE,		MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_W, Shortcut(KS_TOOL_ROTATE,		MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_E, Shortcut(KS_TOOL_ROTATE_Y,	MK_NOMOD)));
	mShortcuts.insert(ShortcutPair(InputSystem::KC_R, Shortcut(KS_TOOL_SCALE,		MK_NOMOD)));
}

KeyShortcuts::~KeyShortcuts()
{

}

KeyShortcuts::eKeyShortcut KeyShortcuts::GetPressedShortcut(InputSystem::eKeyCode keyCode)
{
	uint8 modKeyFlags = MK_NOMOD;
	// checking if alt is pressed
	if (gInputMgr.IsKeyDown(InputSystem::KC_LMENU) || gInputMgr.IsKeyDown(InputSystem::KC_RMENU))
		modKeyFlags |= MK_ALT;
	
	// checking if ctrl is pressed
	if (gInputMgr.IsKeyDown(InputSystem::KC_LCONTROL) || gInputMgr.IsKeyDown(InputSystem::KC_RCONTROL))
		modKeyFlags |= MK_CTRL;
	
	// checking if shift is pressed
	if (gInputMgr.IsKeyDown(InputSystem::KC_LSHIFT) || gInputMgr.IsKeyDown(InputSystem::KC_RSHIFT))
		modKeyFlags |= MK_SHIFT;

	pair<ShortcutMap::iterator, ShortcutMap::iterator> ret = mShortcuts.equal_range(keyCode);

	ShortcutMap::iterator it;
	for (it=ret.first; it!=ret.second; ++it)
	{
		if (it->second.modKeyFlags == modKeyFlags)
			return it->second.shortcutEnum;
	}

	return KeyShortcuts::KS_UKNOWN;
}