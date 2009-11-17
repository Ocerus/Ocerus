#include "TestLogMessage.as"
void OnInit(EntityHandle handle)
{
  uint32 timeOut = handle.Get_uint32("OnInitTimeOut");
  string name = handle.Get_string("OnInitScript");
  Log(PrintLogMessage(name, timeOut));
  array_int32 arrayTest = handle.Get_array_int32("ArrayTest");
  int32 x = arrayTest[2];
  Log("ArrayTest[2] is " + x + ".");
  arrayTest[2] = 5;
  x = arrayTest[2];
  Log("ArrayTest[2] is " + x + ".");
  arrayTest.Resize(4);
  arrayTest[3] = 7;
  const array_int32 arrayTest2 = handle.Get_const_array_int32("ArrayTest");
  int32 y = arrayTest[3];
  Log("ArrayTest[3] is " + y + ".");
  int32 l = arrayTest.GetSize();
  Log("ArrayTest has size " + l + ".");
}