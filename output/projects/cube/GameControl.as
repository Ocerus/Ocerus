void ExitLevel()
{
	EntityHandle director = gEntityMgr.FindFirstEntity("Director");
	if (director.Exists())	
	{
		uint32 totalScore = game.Get_uint32("TotalScore") + director.Get_uint32("Score");
		game.Set_uint32("TotalScore", totalScore);
		float32 totalTime = game.Get_float32("TotalTime") + director.Get_float32("Time");
		game.Set_float32("TotalTime", totalTime);
	}	
	game.SaveToFile("TestGame");
	if (HasNextLevel())
	{
		gProject.OpenScene(GetNextLevel());
	}
	else
	{
		gProject.OpenScene("finish.xml");
	}
}

int GetCurrentLevelIndex()
{
	array_string levels = game.Get_array_string("Levels");
	string currentLevelName = gProject.GetOpenedSceneName();
	for (int i=0; i<levels.GetSize(); ++i)
	{
		if (levels[i] == currentLevelName) return i;
	}
	return -1;
}

int GetLevelCount()
{
	 return game.Get_array_string("Levels").GetSize();
}

string GetNextLevel()
{
	return game.Get_array_string("Levels")[GetCurrentLevelIndex()+1];
}

bool HasNextLevel()
{
	return GetCurrentLevelIndex() < GetLevelCount()-1;
}

void InitGameControl()
{
	array_string levels = game.Get_array_string("Levels");
	if (levels.GetSize() == 0)
	{
		levels.Resize(2);
		levels[0] = "level1.xml";
		levels[1] = "level2.xml";
	}
}

void PauseGame()
{
	game.PauseAction();
}

void ResumeGame()
{
	game.ResumeAction();
}

void RestartGame()
{
	gProject.OpenScene(gProject.GetOpenedSceneName());
}

void QuitGame()
{
	game.Quit();
}

void TryOpenMenu()
{
	EntityHandle gameGui = gEntityMgr.FindFirstEntity("GameGUI");
	EntityHandle menuGui = gEntityMgr.FindFirstEntity("MenuGUI");
	if (!gameGui.Get_bool("Enabled") || !gameGui.Get_bool("Visible")) return;
	
	PauseGame();
	gameGui.Set_bool("Enabled", false);
	menuGui.Set_bool("Visible", true);
}

void TryCloseMenu()
{
	EntityHandle gameGui = gEntityMgr.FindFirstEntity("GameGUI");
	EntityHandle menuGui = gEntityMgr.FindFirstEntity("MenuGUI");
	if (!menuGui.Get_bool("Enabled") || !menuGui.Get_bool("Visible")) return;
	
	menuGui.Set_bool("Visible", false);
	gameGui.Set_bool("Enabled", true);
	ResumeGame();
}

void ToggleMenu()
{
	EntityHandle menuGui = gEntityMgr.FindFirstEntity("MenuGUI");
	if (menuGui.Get_bool("Visible"))	
	{
		TryCloseMenu();
	}
	else
	{
		TryOpenMenu();
	}
}