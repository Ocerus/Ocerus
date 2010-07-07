void SpawnScore10(const Vector2 position)
{
	EntityHandle scoreEntity = gEntityMgr.InstantiatePrototype(gEntityMgr.FindFirstPrototype("Score10"), position, "Score10");
	scoreEntity.Set_float32("FadeAlphaSpeed", 2.0f);
	scoreEntity.Set_float32("FadeMovementSpeed", 0.1f);
}
