void OnUpdateLogic(float32 delta)
{
	EntityHandle player = GetEntityMgr().FindFirstEntity("Player");
	EntityHandle camera = GetEntityMgr().FindFirstEntity("GameCamera");
	camera.Set_Vector2("Position", player.Get_Vector2("Position"));
}

void OnKeyPressed(eKeyCode key, uint32 char)
{

}
