void OnInit()
{
  gGUIMgr.LoadScheme("gui/cube.scheme");
}

void OnUpdateLogic(float32 delta)
{
  EntityHandle director = gEntityMgr.FindFirstEntity("Director");
  if (!director.IsValid()) return;
  
  Window@ scoreLabel = GetWindow("Score");
  if (scoreLabel !is null)
  {
   	uint32 score = director.Get_uint32("Score") + game.Get_uint32("TotalScore");
    scoreLabel.SetText("" + score);
  }

  Window@ timeLabel = GetWindow("Time");
  if (timeLabel !is null)
  {
   	float32 timeInSeconds = director.Get_float32("Time");
   	uint32 timeMinutes = MathUtils::Floor(timeInSeconds / 60);
   	uint32 timeSeconds = MathUtils::Round(timeInSeconds - timeMinutes);
    timeLabel.SetText(timeMinutes + ":" + timeSeconds);
  }
  
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
}