#include "GameControl.as"

void OnInit()
{
	game.SetFullscreen(true);	
}

void OnUpdateLogic(float32 delta)
{
	EntityHandle player = gEntityMgr.FindFirstEntity("Player");
	EntityHandle camera = gEntityMgr.FindFirstEntity("GameCamera");
	Vector2 camera_pos = player.Get_Vector2("Position");
	camera_pos.y = camera_pos.y - 1;
	camera.Set_Vector2("Position", camera_pos );
	camera.Set_float32("Rotation", 0);
	camera.Set_float32("Zoom", 0.7f);
}
