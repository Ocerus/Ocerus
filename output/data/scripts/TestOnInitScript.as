void OnInit(EntityHandle handle)
{
  uint32 timeOut = handle.Get_uint32("OnInitTimeOut");
  string name = handle.Get_string("OnInitScript");
  Log("Time out of script '" + name + "' is " + timeOut + " ms.");
}