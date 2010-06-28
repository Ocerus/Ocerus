void OnInit()
{
  Println("Initializing entity " + this.GetID() + "...");
  this.RegisterDynamicProperty_int32("Counter", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ | PA_EDIT_WRITE, "comment");
}

void OnPostInit()
{
  Println("Entity " + this.GetID() + " was inicialized.");
  if (this.Exists()) this.PostMessage(DRAW);
}

void OnUpdateLogic(float32 delta)
{
  //Println("Update logic: " + delta);
}

void OnDraw()
{
  Println("Drawing entity " + this.GetID() + "...");
  uint32 timeOut = this.Get_uint32("ScriptTimeOut");
  Println("Time out of this script is " + timeOut + " ms.");
}

void OnAction()
{
  switch (GetState())
  {
    case 0:
    {
      this.Set_int32("Counter", 2);
      Println("First call of OnAction(). This function will be called every 10 seconds.");
      SetAndSleep(1, 10000);
      break;
    }
    case 1:
    {
      int32 counter = this.Get_int32("Counter");
      Println("Message no. " + counter + " from OnAction(). Wait 10 seconds for another.");
      this.Set_int32("Counter", counter+1);
      SetAndSleep(1, 10000);
      break;
    }
  }
}

void OnDestroy()
{
  this.UnregisterDynamicProperty("Counter");
  Println("Entity " + this.GetID() + " is being destroyed."); 
}
