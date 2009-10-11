/// @file
/// Iterators to walk across entities and their parts.

#ifndef ComponentIterators_h__
#define ComponentIterators_h__

#include "Base.h"
#include "ComponentEnums.h"

namespace EntitySystem 
{
	
	/// A list of generic components.
	typedef vector<Component*> ComponentsList;

	/// Enables iterating over a collection of components. Wraps around an STL iterator.
	class EntityComponentsIterator : public ComponentsList::const_iterator
	{
	public:

		/// Creates an iterator to walk across entity components in the given list. Beginning with the first item.
		EntityComponentsIterator(ComponentsList* componentsList):
			mComponentsList(componentsList),
			ComponentsList::const_iterator(componentsList->begin()) {}

		/// Creates an iterator to walk across entity components in the given list. Bbeginning with the first item.
		EntityComponentsIterator(ComponentsList& componentsList):
			mComponentsList(&componentsList),
			ComponentsList::const_iterator(componentsList.begin()) {}

		/// Creates an iterator to walk across components in the given list. Beginning with the item determined by
		/// the given iterator.
		EntityComponentsIterator(ComponentsList* componentsList, const ComponentsList::const_iterator firstItem):
			mComponentsList(componentsList),
			ComponentsList::const_iterator(firstItem) {}

		/// Copy constructor.
		EntityComponentsIterator(const EntityComponentsIterator& rhs):
			mComponentsList(rhs.mComponentsList),
			ComponentsList::const_iterator(rhs) {}

		virtual ~EntityComponentsIterator(void) {}

		EntityComponentsIterator& operator=(const EntityComponentsIterator& rhs)
		{
			mComponentsList = rhs.mComponentsList;
			ComponentsList::const_iterator::operator=(rhs);
		}

		/// Use this method as a loop condition instead of comparing the iterator to a collection end().
		bool HasMore(void) const { return (*this) != mComponentsList->end(); }

		/// Returns a reference to the list owner.
		inline const ComponentsList& GetList(void) const { return *mComponentsList; }

		/// Returns an end iterator of the owning list.
		EntityComponentsIterator GetEnd(void) const
		{
			return EntityComponentsIterator(mComponentsList, mComponentsList->end());
		}

	private:
		ComponentsList* mComponentsList;
	};

}

#endif // ComponentIterators_h__
