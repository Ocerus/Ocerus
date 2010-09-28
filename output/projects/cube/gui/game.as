float32 TitleAnimState;

void OnPostInit()
{
  TitleAnimState = 0.0;
  GetWindow("LevelTitle").SetProperty("Alpha", "0.0");
  //TODO get level name or index
  int32 levelIndex = 1;
  GetWindow("LevelTitle").SetText(GetTextData("GUI", "levelTitle") + levelIndex);
}

void OnUpdateLogic(float32 delta)
{
  EntityHandle director = gEntityMgr.FindFirstEntity("Director");
  if (!director.IsValid()) return;
  
 	uint32 score = director.Get_uint32("Score") + game.Get_uint32("TotalScore");
  GetWindow("Score").SetText("" + score);

 	float32 timeInSeconds = director.Get_float32("Time");
 	uint32 timeMinutes = MathUtils::Floor(timeInSeconds / 60);
 	uint32 timeSeconds = MathUtils::Round(timeInSeconds - timeMinutes);
  GetWindow("Time").SetText(timeMinutes + ":" + timeSeconds);
  
  EntityHandle player = gEntityMgr.FindFirstEntity("Player");
  Window@ stateButton = GetWindow("StateButton");
  if (player.IsValid() && (stateButton !is null))
  {
  	if (player.Get_bool("IsLight"))
  	{
 			stateButton.SetProperty("NormalImage", "set:Buttons image:CubeLight");
 		}
 		else
 		{
 			stateButton.SetProperty("NormalImage", "set:Buttons image:CubeHeavy");
 		}
  }
  

  if (TitleAnimState < 3.0)
  {
  	if (TitleAnimState > 1.0 && TitleAnimState <= 2.0) TitleAnimState = TitleAnimState + 0.01f;
  	else TitleAnimState = TitleAnimState + 0.03f;

  	if (TitleAnimState > 3.0) TitleAnimState = 3.0;

  	if (TitleAnimState <= 1.0)
		{
			GetWindow("LevelTitle").SetProperty("Alpha", "" + TitleAnimState);
		}
		else if (TitleAnimState <= 2.0)
		{
			GetWindow("LevelTitle").SetProperty("Alpha", "1.0");
		}
		else
		{
			GetWindow("LevelTitle").SetProperty("Alpha", "" + (3.0-TitleAnimState));
		}
  }
}