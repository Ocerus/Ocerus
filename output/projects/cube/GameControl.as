void ExitLevel()
{
	EntityHandle director = gEntityMgr.FindFirstEntity("Director");
	if (director.Exists())	
	{
		uint32 total = game.Get_uint32("TotalScore") + director.Get_uint32("Score");
		game.Set_uint32("TotalScore", total);
	}	
	int32 sceneIndex = gProject.GetSceneIndex(gProject.GetOpenedSceneName()) + 1;
	gProject.OpenSceneAtIndex(sceneIndex != int32(gProject.GetSceneCount()) ? sceneIndex : 0);
	game.SaveToFile("TestGame");
}
