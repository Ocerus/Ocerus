void OnPostInit()
{
  this.RegisterDynamicProperty_float32("SizeOffsetAmplitude", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("SizeOffsetPeriod", PA_FULL_ACCESS, "");

  this.RegisterDynamicProperty_Vector2("SizeOffsetInitial", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");
  this.RegisterDynamicProperty_float32("SizeOffsetTime", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");

	this.Set_Vector2("SizeOffsetInitial", this.Get_Vector2("Scale"));  
  this.Set_float32("SizeOffsetTime", 0);
}

void OnUpdateLogic(float32 delta)
{
	this.Set_float32("SizeOffsetTime", this.Get_float32("SizeOffsetTime") + delta);
	float32 targetOffset = this.Get_float32("SizeOffsetAmplitude") * MathUtils::Sin(this.Get_float32("SizeOffsetTime") / this.Get_float32("SizeOffsetPeriod"));
	this.Set_Vector2("Scale", this.Get_Vector2("SizeOffsetInitial") + Vector2(targetOffset, targetOffset));
}
