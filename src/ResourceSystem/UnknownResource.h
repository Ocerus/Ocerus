/// @file
/// Derived class from Resource representing a resource of an unknown type.

#ifndef _UNKNOWRESOURCE_H_
#define _UNKNOWRESOURCE_H_

#include "Base.h"
#include "Resource.h"

namespace ResourceSystem
{
	/// Resource class representing a resource of an unknown type.
	class UnknownResource : public ResourceSystem::Resource
	{
	public:

		/// Default destructor.
		virtual ~UnknownResource(void){}

		/// Returns a pointer to a new instance
		static ResourceSystem::ResourcePtr CreateMe(void){ return ResourcePtr(new UnknownResource()); }

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_UNKNOWN; }

	protected:
		virtual size_t LoadImpl(void){ ocWarning << "Trying to load unknown resource !!!"; return 0; }
		virtual bool UnloadImpl(void){ return true; }
	};
}

#endif