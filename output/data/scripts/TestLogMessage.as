string PrintLogMessage(string name, uint32 timeOut)
{
  string temp = "Time out of script '" + name + "' is " + timeOut + " ms.";
  #if TEST
    temp = "Test: " + temp; 
  #endif
  return temp;
}