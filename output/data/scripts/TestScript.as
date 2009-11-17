void OnInit()
{
  EntityHandle handle = getCurrentEntityHandle();
  uint32 timeOut = handle.Get_uint32("ScriptTimeOut");
  Log("Time out of this script is " + timeOut + " ms.");
  const array_string modules = handle.Get_const_array_string("ScriptModules");
  for (int32 i = 0; i<modules.GetSize(); ++i)
  {
    Log("Module number " + (i+1) + " is '" + modules[i] +"'.");
  }
}