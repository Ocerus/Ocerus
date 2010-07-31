/// @file
/// Encapsulation of property map.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef PropertyMap_h__
#define PropertyMap_h__

#include "Base.h"
#include "PropertyAccess.h"

namespace Reflection
{
	/// A list of generic properties.
	typedef vector<AbstractProperty*> AbstractPropertyList;

	/// A map of abstract properties.
	typedef hash_map<StringKey, AbstractProperty*> AbstractPropertyMap;

	/// The properties map encapsulation.
	class PropertyMap
	{
	public:

		/// Fills a data structure with all properties.
		void EnumProperties( AbstractPropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS ) const;

		/// Fills a data structure with all properties.
		void EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS ) const;

		/// Returns a property identified by it's string key. Access restriction filter can be defined.
		AbstractProperty* GetProperty(const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns true if the property exists.
		bool HasProperty(const StringKey key) const;

		/// Adds a property.
		bool AddProperty(AbstractProperty* prop);

		/// Deletes a property. It does not release its memory.
		bool DeleteProperty(const StringKey key);
		
		/// Clears all properties. It does not release theirs memory.
		void ClearProperties();

	private:
		AbstractPropertyMap mProperties;
	};
}

#endif //PropertyMap_h__