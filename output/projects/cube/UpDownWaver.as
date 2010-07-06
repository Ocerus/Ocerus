void OnPostInit()
{
  this.RegisterDynamicProperty_float32("YOffsetAmplitude", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("YOffsetPeriod", PA_FULL_ACCESS, "");

  this.RegisterDynamicProperty_Vector2("YOffsetPivotPosition", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");
  this.RegisterDynamicProperty_float32("YOffsetTime", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");

	this.Set_Vector2("YOffsetPivotPosition", this.Get_Vector2("Position"));  
  this.Set_float32("YOffsetTime", 0);
}

void OnUpdateLogic(float32 delta)
{
	this.Set_float32("YOffsetTime", this.Get_float32("YOffsetTime") + delta);
	float32 targetOffset = this.Get_float32("YOffsetAmplitude") * MathUtils::Sin(this.Get_float32("YOffsetTime") / this.Get_float32("YOffsetPeriod"));
	this.Set_Vector2("Position", this.Get_Vector2("YOffsetPivotPosition") + Vector2(0, targetOffset));
}
