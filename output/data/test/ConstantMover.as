void OnUpdatePhysics(float32 delta)
{
  EntityHandle handle = GetCurrentEntityHandle();
	handle.CallFunction("ApplyForce", PropertyFunctionParameters() << Vector2(100000.0, 0.0) << Vector2(0, 0));
}