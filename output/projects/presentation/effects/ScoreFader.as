void OnPostInit()
{
  this.RegisterDynamicProperty_float32("FadeAlphaSpeed", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("FadeMovementSpeed", PA_FULL_ACCESS, "");
}

void OnDraw(float32 delta)
{
	if (this.Get_float32("Transparency") < 1.0f)
	{
		this.Set_float32("Transparency", MathUtils::Min(1.0f, this.Get_float32("Transparency") + delta * this.Get_float32("FadeAlphaSpeed")));
		this.Set_Vector2("Position", this.Get_Vector2("Position") + Vector2(0, -this.Get_float32("FadeMovementSpeed")));
	}
	else
	{
		gEntityMgr.DestroyEntity(this);
	}
}