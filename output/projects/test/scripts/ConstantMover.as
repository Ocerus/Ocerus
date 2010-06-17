void OnUpdateLogic(float32 delta)
{
	EntityHandle handle = GetCurrentEntityHandle();
	//handle.CallFunction("ApplyForce", PropertyFunctionParameters() << Vector2(100000.0, 0.0) << Vector2(0, 0));
	/*Vector2 pos = handle.Get_Vector2("Position");
	pos.x = pos.x + 0.5;
	handle.Set_Vector2("Position", pos);*/
}

void OnKeyPressed(eKeyCode key, uint32 char)
{
	Println("key pressed");
}

void OnKeyReleased(eKeyCode key, uint32 char)
{
	Println("key released");
}
