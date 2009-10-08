/// @file
/// Definition of access filters to properties.

#ifndef PropertyAccess_h__
#define PropertyAccess_h__

namespace Reflection
{
	/// @brief Restrictions of access which can be granted to a property.
	/// @remarks The property can be access from scripts, from editor, during the init, ... By using these values you can
	/// define a filter and forbid to access certain properties from certain parts of the code.
	///	Note that it is assumed that this type fits into PropertyAccessFlags!
	enum ePropertyAccess 
	{ 
		/// The property can be read from edit tools.
		PROPACC_EDIT_READ=1<<1, 
		/// The property can be written from edit tools.
		PROPACC_EDIT_WRITE=1<<2, 
		/// The property can be read from scripts.
		PROPACC_SCRIPT_READ=1<<3, 
		/// The property can be written from scripts.
		PROPACC_SCRIPT_WRITE=1<<4,
		/// The property can be written during the component init.
		PROPACC_INIT=1<<5,

		/// Full access to the property.
		PROPACC_FULL=0xff
	};

	/// Type for storing access flags defined by ePropertyAccess.
	typedef uint8 PropertyAccessFlags;
}

#endif // PropertyAccess_h__