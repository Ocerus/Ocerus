#include "GameControl.as"
#include "gui/textbox.as"

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
	TextboxSetText(GetTextData("GUI", "helpText"));
	gEntityMgr.FindFirstEntity("TextboxGUI").Set_bool("Visible", true);
	this.Set_bool("Enabled", false);
}

void OnQuitClicked(Window@ wnd)
{
	QuitGame();
}