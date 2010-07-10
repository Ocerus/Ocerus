void SpawnScore10(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("Score10"), position, "Score10");
}

void SpawnJumpDust(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("JumpDust"), position, "JumpDust");
}
