#include "GameControl.as"
#include "gui/textbox.as"

void OnContinueClicked(Window@ wnd)
{
	TryCloseMenu();
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

void OnKeyPressed(eKeyCode key, uint32 char)
{
	if (key == KC_ESCAPE)
	{
		ToggleMenu();
	}
}
