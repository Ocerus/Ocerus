void OnUpdateLogic(float32 delta)
{
	EntityHandle player = entityMgr.FindFirstEntity("Player");
	EntityHandle camera = entityMgr.FindFirstEntity("GameCamera");
	camera.Set_Vector2("Position", player.Get_Vector2("Position"));
}

void OnKeyPressed(eKeyCode key, uint32 char)
{

}
