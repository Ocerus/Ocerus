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
