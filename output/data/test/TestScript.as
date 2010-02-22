void OnInit()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Log("Inicializing entity " + handle.GetID() + "...");
  const array_string modules = handle.Get_const_array_string("ScriptModules");
  for (int32 i = 0; i<modules.GetSize(); ++i)
  {
    Log("Module number " + (i+1) + " is '" + modules[i] +"'.");
  }
  handle.RegisterDynamicProperty_int32(StringKey("Counter"), PA_SCRIPT_READ | PA_SCRIPT_WRITE, "comment");
}

void OnPostInit()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Log("Entity " + handle.GetID() + " was inicialized.");
  if (GetEntityMgr().EntityExists(handle)) handle.PostMessage(DRAW);
}

void OnUpdateLogic(float32 delta)
{
	//Log("Update logic: " + delta);
}

void OnDraw()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Log("Drawing entity " + handle.GetID() + "...");
  uint32 timeOut = handle.Get_uint32("ScriptTimeOut");
  Log("Time out of this script is " + timeOut + " ms.");
}

void OnAction()
{
  EntityHandle handle = GetCurrentEntityHandle();
  switch (GetState())
  {
    case 0:
    {
      handle.Set_int32("Counter", 2);
      Log("First call of OnAction(). This function will be called every 10 seconds.");
      SetAndSleep(1, 10000);
      break;
    }
    case 1:
    {
      int32 counter = handle.Get_int32("Counter");
      Log("Message no. " + counter + " from OnAction(). Wait 10 seconds for another.");
      handle.Set_int32("Counter", counter+1);
      SetAndSleep(1, 10000);
      break;
    }
  }
}