void OnInit(EntityHandle handle)
{
  uint32 timeOut = handle.Get_uint32("OnInitTimeOut");
  Log("Time out of this script is " + timeOut + ".");
}