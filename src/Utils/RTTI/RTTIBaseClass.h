#ifndef _BASEOBJECT_H_
#define _BASEOBJECT_H_

#include "RTTIGlue.h"

/** @name RTTINullClass is an empty class with no attached RTTI information. We use this class as an ancestor
	to our RTTIBaseClass which is the true ultimate ancestor for all our classes. Without the null class
	we would have no templater parameter for RTTIBaseClass's ancestor. RTTINullClass itself has no RTTI support.
*/
class RTTINullClass
{
protected :

	/** @name It is necessary to implement this to avoid compilation errors in the templatized RTTI code
		In any case no RTTI support is provided from this class
	*/
	static inline RTTI* GetClassRTTI(void) { return NULL;	}

};

/** @name Ultimate ancestor for all classes. RTTIBaseClass implements no specific functionality, apart from
	the fact it provides the ultimate ancestor RTTI for all other classes.
*/
class RTTIBaseClass : public RTTIGlue<RTTIBaseClass, RTTINullClass>
{

};

#endif	// _BASEOBJECT_H_