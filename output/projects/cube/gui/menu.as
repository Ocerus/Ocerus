#include "GameControl.as"

void OnPostInit()
{
	this.Set_bool("Visible", false);
}

void OnContinueClicked(Window@ wnd)
{
	this.Set_bool("Visible", false);
	gEntityMgr.FindFirstEntity("GameGUI").Set_bool("Enabled", true);
	ResumeGame();
}

void OnRestartClicked(Window@ wnd)
{
	RestartGame();
}

void OnHelpClicked(Window@ wnd)
{
	Println("help");
}

void OnQuitClicked(Window@ wnd)
{
	QuitGame();
}