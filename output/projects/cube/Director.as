void OnUpdateLogic(float32 delta)
{
	EntityHandle player = gEntityMgr.FindFirstEntity("Player");
	EntityHandle camera = gEntityMgr.FindFirstEntity("GameCamera");
	camera.Set_Vector2("Position", player.Get_Vector2("Position"));
}

void OnKeyPressed(eKeyCode key, uint32 char)
{

}
