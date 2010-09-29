#include "player/GuiControl.as"

void OnStateButtonClicked(Window@ wnd)
{
	SwitchMode(gEntityMgr.FindFirstEntity("Player"));
}

void OnJumpButtonClicked(Window@ wnd)
{
	TryJump(gEntityMgr.FindFirstEntity("Player"));
}

void OnExplosionButtonClicked(Window@ wnd)
{
	TryExplode(gEntityMgr.FindFirstEntity("Player"));
}
