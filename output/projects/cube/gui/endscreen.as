#include "GameControl.as"

void OnPostInit()
{
	GetWindow("TotalScore/Value").SetText("" + game.Get_uint32("TotalScore"));
	GetWindow("TotalTime/Value").SetText("" + game.Get_float32("TotalTime"));
}

void OnQuitClicked(Window@ wnd)
{
	QuitGame();
}