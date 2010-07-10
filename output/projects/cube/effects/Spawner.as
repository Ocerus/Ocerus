void SpawnScore10(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("Score10"), position, "Score10");
	entity.Set_float32("FadeAlphaSpeed", 2.0f);
	entity.Set_float32("FadeMovementSpeed", 0.1f);
}

void SpawnJumpDust(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("JumpDust"), position, "JumpDust");
	Println("Dust spawned");
}
