#include "Common.h"
#include "InputActions.h"

const char* InputSystem::GetKeyCodeString(uint8 code)
{
  switch (code)
  {
    #define KEY_CODE(x, y) case y: return #x ;
		#include "KeyCodes.h"
		#undef KEY_CODE
		
    default: break;
  }
  return 0;
}