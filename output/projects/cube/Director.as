#include "GameControl.as"

void OnUpdateLogic(float32 delta)
{
	EntityHandle player = gEntityMgr.FindFirstEntity("Player");
	EntityHandle camera = gEntityMgr.FindFirstEntity("GameCamera");
	camera.Set_Vector2("Position", player.Get_Vector2("Position"));
	camera.Set_float32("Rotation", 0);
	camera.Set_float32("Zoom", 1.0f);
}
