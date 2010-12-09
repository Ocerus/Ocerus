#include "player/GuiControl.as"
#include "GameControl.as"

float32 TitleAnimState;

void OnPostInit()
{
  TitleAnimState = 0.0;
  GetWindow("GameLayout/LevelTitle").SetProperty("Alpha", "0.0");
  int32 levelIndex = GetCurrentLevelIndex();
  GetWindow("GameLayout/LevelTitle").SetText(GetTextData("GUI", "levelTitle") + (levelIndex+1));
}

void OnUpdateLogic(float32 delta)
{
  EntityHandle director = gEntityMgr.FindFirstEntity("Director");
  if (!director.IsValid()) return;
  
  //uint32 score = director.Get_uint32("Score") + game.Get_uint32("TotalScore");
  //GetWindow("GameLayout/Score").SetText("" + score);

  float32 timeInSeconds = director.Get_float32("Time");
  uint32 timeMinutes = MathUtils::Floor(timeInSeconds / 60);
  uint32 timeSeconds = MathUtils::Round(timeInSeconds - timeMinutes * 60);
  GetWindow("GameLayout/Time").SetText(timeMinutes + ":" + ((timeSeconds < 10) ? "0" : "") + timeSeconds);
  
  UpdateButtons();
  LevelTitleAnimation(delta);
}

void UpdateButtons()
{
  EntityHandle player = gEntityMgr.FindFirstEntity("Player");
  if (!player.IsValid()) return;
  
 	if (player.Get_bool("IsLight"))
 	{
		SetButtonSingleImage("GameLayout/StateButton", "CubeLight");
 	}
 	else
 	{
		SetButtonSingleImage("GameLayout/StateButton", "CubeHeavy");
 	}
 	
 	UpdateSkillButton(CanJump(player), "Jump");
 	UpdateSkillButton(CanExplode(player), "Explosion");
}

void UpdateSkillButton(bool enabled, string skill)
{
 	if (enabled)
 	{
 		SetButtonAllImages("GameLayout/" + skill + "Button", skill);
 		GetWindow("GameLayout/" + skill + "Cooldown").SetVisible(false);
 	}
 	else
 	{
 		SetButtonSingleImage("GameLayout/" + skill + "Button", skill + "Off");
 		float32 cooldown = MathUtils::Ceiling(gEntityMgr.FindFirstEntity("Player").Get_float32(skill + "Cooldown"));
 		if (cooldown > 0)
 		{
			GetWindow("GameLayout/" + skill + "Cooldown").SetText("" + cooldown);
 			GetWindow("GameLayout/" + skill + "Cooldown").SetVisible(true);
 		}
 		else
 		{
 			GetWindow("GameLayout/" + skill + "Cooldown").SetVisible(false);
 		}
 	}
}

void SetButtonSingleImage(string button, string image)
{
	Window@ wnd = GetWindow(button);
	wnd.SetProperty("NormalImage", "set:Buttons image:" + image);
	wnd.SetProperty("HoverImage", "set:Buttons image:" + image);
	wnd.SetProperty("PushedImage", "set:Buttons image:" + image);
	wnd.SetProperty("DisabledImage", "set:Buttons image:" + image);	
}

void SetButtonAllImages(string button, string image)
{
	Window@ wnd = GetWindow(button);
	wnd.SetProperty("NormalImage", "set:Buttons image:" + image + "On");
	wnd.SetProperty("HoverImage", "set:Buttons image:" + image + "Hover");
	wnd.SetProperty("PushedImage", "set:Buttons image:" + image + "Off");
	wnd.SetProperty("DisabledImage", "set:Buttons image:" + image + "Off");
}

void LevelTitleAnimation(float32 delta)
{
  if (TitleAnimState < 3.0)
  {
  	if (TitleAnimState > 1.0 && TitleAnimState <= 2.0) TitleAnimState = TitleAnimState + 0.01f;
  	else TitleAnimState = TitleAnimState + 0.03f;

  	if (TitleAnimState > 3.0) TitleAnimState = 3.0;

  	if (TitleAnimState <= 1.0)
		{
			GetWindow("GameLayout/LevelTitle").SetProperty("Alpha", "" + TitleAnimState);
		}
		else if (TitleAnimState <= 2.0)
		{
			GetWindow("GameLayout/LevelTitle").SetProperty("Alpha", "1.0");
		}
		else
		{
			GetWindow("GameLayout/LevelTitle").SetProperty("Alpha", "" + (3.0-TitleAnimState));
		}
  }
}

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

void OnMenuButtonClicked(Window@ wnd)
{
	TryOpenMenu();
}
