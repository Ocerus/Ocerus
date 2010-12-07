void OnPostInit()
{
  this.RegisterDynamicProperty_string("TextName", PA_FULL_ACCESS, "");
}

void OnUpdateLogic(float32 delta)
{
	EntityHandle text = gEntityMgr.FindFirstEntity(this.Get_string("TextName"));
	Vector2 pos = this.Get_Vector2("Position");
	pos.y = pos.y - 1.5;
	text.Set_Vector2("Position", pos);
}
