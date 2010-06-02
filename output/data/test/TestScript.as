void OnInit()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Println("Initializing entity " + handle.GetID() + "...");
  const array_string modules = handle.Get_const_array_string("ScriptModules");
  for (int32 i = 0; i<modules.GetSize(); ++i)
  {
    Println("Module number " + (i+1) + " is '" + modules[i] +"'.");
  }
  handle.RegisterDynamicProperty_int32("Counter", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ | PA_EDIT_WRITE, "comment");
}

void OnPostInit()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Println("Entity " + handle.GetID() + " was inicialized.");
  if (GetEntityMgr().EntityExists(handle)) handle.PostMessage(DRAW);
}

void OnUpdateLogic(float32 delta)
{
  //Println("Update logic: " + delta);
}

void OnDraw()
{
  EntityHandle handle = GetCurrentEntityHandle();
  Println("Drawing entity " + handle.GetID() + "...");
  uint32 timeOut = handle.Get_uint32("ScriptTimeOut");
  Println("Time out of this script is " + timeOut + " ms.");
}

void OnAction()
{
  EntityHandle handle = GetCurrentEntityHandle();
  switch (GetState())
  {
    case 0:
    {
      handle.Set_int32("Counter", 2);
      Println("First call of OnAction(). This function will be called every 10 seconds.");
      SetAndSleep(1, 10000);
      break;
    }
    case 1:
    {
      int32 counter = handle.Get_int32("Counter");
      Println("Message no. " + counter + " from OnAction(). Wait 10 seconds for another.");
      handle.Set_int32("Counter", counter+1);
      SetAndSleep(1, 10000);
      break;
    }
  }
}

void OnDestroy()
{
  EntityHandle handle = GetCurrentEntityHandle();
  handle.UnregisterDynamicProperty("Counter");
  Println("Entity " + handle.GetID() + " is being destroyed."); 
}
