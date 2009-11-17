void OnInit(EntityHandle handle)
{
  uint32 timeOut = handle.Get_uint32("TimeOut");
  Log("Time out of this script is " + timeOut + " ms.");
  const array_string modules = handle.Get_const_array_string("Modules");
  for (int32 i = 0; i<modules.GetSize(); ++i)
  {
    Log("Module number " + (i+1) + " is '" + modules[i] +"'.");
  }
}