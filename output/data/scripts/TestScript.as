void OnInit()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Log("Inicializing entity " + handle.GetID() + "...");
  const array_string modules = handle.Get_const_array_string("ScriptModules");
  for (int32 i = 0; i<modules.GetSize(); ++i)
  {
    Log("Module number " + (i+1) + " is '" + modules[i] +"'.");
  }
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
  switch (GetState())
  {
    case 0:
    {
      Log("First call of OnAction(). This function will be called every 10 seconds.");
      SetAndSleep(1, 10000);
      break;
    }
    case 1:
    {
      Log("Message from OnAction(). Wait 10 seconds for another.");
      SetAndSleep(1, 10000);
      break;
    }
  }
}