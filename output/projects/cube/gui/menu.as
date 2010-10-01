#include "GameControl.as"

void OnPostInit()
{
	gEntityMgr.FindFirstEntity("MenuGUI").Set_bool("Visible", false);
}

void OnContinueClicked(Window@ wnd)
{
	gEntityMgr.FindFirstEntity("MenuGUI").Set_bool("Visible", false);
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