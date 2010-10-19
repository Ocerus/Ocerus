#include "GameControl.as"

void OnPostInit()
{
	GetWindow("TotalScore/Value").SetText("" + game.Get_uint32("TotalScore"));

  float32 timeInSeconds = game.Get_float32("TotalTime");
  uint32 timeMinutes = MathUtils::Floor(timeInSeconds / 60);
  uint32 timeSeconds = MathUtils::Round(timeInSeconds - timeMinutes * 60);
	GetWindow("TotalTime/Value").SetText(timeMinutes + ":" + ((timeSeconds < 10) ? "0" : "") + timeSeconds);
}

void OnQuitClicked(Window@ wnd)
{
	QuitGame();
}