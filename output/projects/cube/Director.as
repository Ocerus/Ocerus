void OnUpdateLogic(float32 delta)
{
	EntityHandle player = GetEntityMgr().FindFirstEntity("Player");
	EntityHandle camera = GetEntityMgr().FindFirstEntity("GameCamera1");
	camera.Set_Vector2("Position", player.Get_Vector2("Position"));
}

void OnKeyPressed(eKeyCode key, uint32 char)
{

}
