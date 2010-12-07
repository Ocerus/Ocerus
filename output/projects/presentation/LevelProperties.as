#include "GameControl.as"

void OnPostInit()
{
	InitGameControl();

  this.RegisterDynamicProperty_uint32("Score", PA_FULL_ACCESS, "Score the player got so far in this level.");
  this.RegisterDynamicProperty_float32("Time", PA_FULL_ACCESS, "Time in seconds.");
  
  this.Set_uint32("Score", 0);
  this.Set_float32("Time", 0);
}

void OnUpdateLogic(float32 delta)
{
	this.Set_float32("Time", this.Get_float32("Time") + delta);
}
