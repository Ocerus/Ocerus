void ExitLevel()
{
	EntityHandle director = gEntityMgr.FindFirstEntity("Director");
	if (director.Exists())	
	{
		uint32 total = game.Get_uint32("TotalScore") + director.Get_uint32("Score");
		game.Set_uint32("TotalScore", total);
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