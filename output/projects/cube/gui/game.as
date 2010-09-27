void OnInit()
{
  gGUIMgr.LoadScheme("gui/cube.scheme");
  gGUIMgr.LoadImageset("gui/buttons.imageset");
}

void OnUpdateLogic(float32 delta)
{
  Window@ scoreLabel = GetWindow("Score");
  if (scoreLabel !is null)
  {
    EntityHandle director = gEntityMgr.FindFirstEntity("Director");
    if (director.Exists())
    {
    	uint32 score = director.Get_uint32("Score") + game.Get_uint32("TotalScore");
      scoreLabel.SetText(GetTextData("GUI", "score") + score);
    }
  }
}