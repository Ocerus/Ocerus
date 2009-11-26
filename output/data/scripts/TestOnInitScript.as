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

  PropertyFunctionParameters pfp1;
  pfp1.Push_string(name);
  pfp1.Push_int32(timeOut);
  handle.CallFunction("TestFunction", pfp1);

  PropertyFunctionParameters pfp2;
  pfp2.Push_string("cislo 7");
  {
    int32 test = 7;
    pfp2.Push_int32(test);
  }
  int32 a = 546;
  string b = "ahoj";
  handle.CallFunction("TestFunction", pfp2);

  PropertyFunctionParameters pfp3;
  pfp3 << "test <<" << int32(1);
  handle.CallFunction("TestFunction", pfp3);

  handle.CallFunction("TestFunction", PropertyFunctionParameters() << "test <<" << int32(2));

  Color color(1, 2, 3, 4);
  Log("R: " + color.r + ", G: " + color.g + ", B: " + color.b + ", A: " + color.a);
  color.r = 5;
  color.g = 6;
  Log("R: " + color.r + ", G: " + color.g + ", B: " + color.b + ", A: " + color.a);

  Vector2 vector(1, 2);
  Log("X: " + vector.x + ", Y: " + vector.y);
  vector.x = vector.x + 2;
  vector.y = vector.y + 3;
  vector *= 2; 
  Log("X: " + vector.x + ", Y: " + vector.y);
}