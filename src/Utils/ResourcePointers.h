/// @file
/// This file gathers smart pointers of all resources together so that it can be included with no overhead.

#ifndef ResourcePointers_h__
#define ResourcePointers_h__

#include "SmartAssert.h"
#include "ResourceSystem/Resource.h"
#include "LogSystem/LogMacros.h"
#include <boost/shared_ptr.hpp>

namespace ResourceSystem
{
	class Resource;
	class XMLResource;

	/// Smart pointer to Resource. This pointer allows type-checked casting to
	/// specific resource pointers.
	class ResourcePtr: public boost::shared_ptr<ResourceSystem::Resource>
	{
	public:
		/// Constructs an empty ResourcePtr.
		ResourcePtr() {}

		/// Constructs a ResourcePtr for given (raw) resource pointer.
		ResourcePtr(ResourceSystem::Resource* rhs): boost::shared_ptr<ResourceSystem::Resource>(rhs) {}

		/// Converts the ResourcePtr to concrete (shared) resource pointer.
		/// This method checks whether the conversion is valid, and returns
		/// null pointer in case of invalid conversion.
		template <class T>
		operator boost::shared_ptr<T>()
		{
			if (!get())
				return boost::shared_ptr<T>();
			if (T::GetResourceType() != (*this)->GetType())
			{
				ocWarning << "Trying to convert resource '" << (*this)->GetName() << "' of type " <<
					ResourceSystem::GetResourceTypeName((*this)->GetType()) << " to " <<
					ResourceSystem::GetResourceTypeName(T::GetResourceType()) << ".";
				return boost::shared_ptr<T>();
			}
			return boost::static_pointer_cast<T>(*this);
		}
	};

	/// Smart pointer to XMLResource.
    typedef boost::shared_ptr<XMLResource> XMLResourcePtr;
}

namespace GUISystem
{
	class CEGUIResource;

	/// Smart pointer to CEGUIResource.
    typedef boost::shared_ptr<CEGUIResource> CEGUIResourcePtr;
}

namespace ScriptSystem
{
	class ScriptResource;

	/// Smart pointer to ScriptResource.
    typedef boost::shared_ptr<ScriptResource> ScriptResourcePtr;
}

namespace GfxSystem
{
	class Texture;
	class Mesh;

	/// Smart pointer to Texture.
	typedef boost::shared_ptr<Texture> TexturePtr;

	/// Smart pointer to Mesh.
	typedef boost::shared_ptr<Mesh> MeshPtr;
}

namespace StringSystem
{
	class TextResource;

	/// Smart pointer to TextResource.
	typedef boost::shared_ptr<TextResource> TextResourcePtr;
}


#endif // ResourcePointers_h__