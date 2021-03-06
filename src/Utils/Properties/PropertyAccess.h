/// @file
/// Definition of access filters to properties.

#ifndef PropertyAccess_h__
#define PropertyAccess_h__

namespace Reflection
{
	/// Restrictions of access which can be granted to a property.
	/// The property can be access from scripts, from editor, during the init, ... By using these values you can
	/// define a filter and forbid to access certain properties from certain parts of the code.
	///	Note that it is assumed that this type fits into PropertyAccessFlags!
	enum ePropertyAccess 
	{ 
		/// The property is accessible only from the C++ code. This must be explicitly stated as otherwise the mask would
		/// not work on the property. That's also why zero flag has no meaning.
		PA_NONE=1<<0, 
		/// The property can be read from edit tools.
		PA_EDIT_READ=1<<1, 
		/// The property can be written from edit tools.
		PA_EDIT_WRITE=1<<2, 
		/// The property can be read from scripts.
		PA_SCRIPT_READ=1<<3, 
		/// The property can be written from scripts.
		PA_SCRIPT_WRITE=1<<4,
		/// The property can be written during the component init.
		PA_INIT=1<<5,
		/// The property should not be loaded/saved from/to stream
		PA_TRANSIENT=1<<6,

		/// Full access to the property.
		PA_FULL_ACCESS=(1<<6) - 1
	};

	/// Type for storing access flags defined by ePropertyAccess.
	typedef uint8 PropertyAccessFlags;
}

#endif // PropertyAccess_h__