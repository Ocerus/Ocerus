void SpawnScore10(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("Score10"), position, "Score10");
}

void SpawnJumpDust(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("JumpDust"), position, "JumpDust");
}

void SpawnExplosionDust(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("ExplosionDust"), position, "ExplosionDust");
}

void SpawnExplosion(const Vector2 position)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("Explosion"), position, "Explosion");
}

//MUHE
void SpawnSmallStone(const Vector2 position, const Vector2 scale, const Vector2 impulse)
{
	EntityHandle entity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("Stone2"), position, "Stone2");
	entity.Set_Vector2("Scale", scale);
	entity.Set_int32("Layer", 1);
	entity.CallFunction("ApplyLinearImpulse", PropertyFunctionParameters() << impulse);
}
