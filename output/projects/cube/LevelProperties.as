void OnPostInit()
{
  this.RegisterDynamicProperty_uint32("Score", PA_FULL_ACCESS, "");
  
  this.Set_uint32("Score", 0);
}
