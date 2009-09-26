#ifndef ComponentIterators_h__
#define ComponentIterators_h__

#include "../../Utility/Settings.h"
#include "ComponentEnums.h"

namespace EntitySystem 
{

	/// @name Forward declarations
	//@{
	class Component;
	//@}
	
	/// @name A list of generic component.
	typedef Vector<Component*> ComponentsList;

	/** Enables iterating over a collection of components. Wraps around an STL iterator.
	*/
	class EntityComponentsIterator : public ComponentsList::const_iterator
	{
	public:
		EntityComponentsIterator(ComponentsList* componentsList):
			mComponentsList(componentsList),
			ComponentsList::const_iterator(componentsList->begin()) {}

		EntityComponentsIterator(ComponentsList& componentsList):
			mComponentsList(&componentsList),
			ComponentsList::const_iterator(componentsList.begin()) {}

		EntityComponentsIterator(ComponentsList* componentsList, const ComponentsList::const_iterator iter):
			mComponentsList(componentsList),
			ComponentsList::const_iterator(iter) {}

		EntityComponentsIterator(const EntityComponentsIterator& rhs):
			mComponentsList(rhs.mComponentsList),
			ComponentsList::const_iterator(rhs) {}

		virtual ~EntityComponentsIterator(void) {}

		EntityComponentsIterator& operator=(const EntityComponentsIterator& rhs)
		{
			mComponentsList = rhs.mComponentsList;
			ComponentsList::const_iterator::operator=(rhs);
		}

		/// @name Use this method as a loop condition instead of comparing the iterator to a collection end().
		bool HasMore(void) { return operator!=(mComponentsList->end()); }

		/// @name Returns a reference to the list owner.
		inline const ComponentsList& GetList(void) const { return *mComponentsList; }

		/// @name Returns an end iterator of the owning list.
		EntityComponentsIterator GetEnd(void) const
		{
			return EntityComponentsIterator(mComponentsList, mComponentsList->end());
		}

	private:
		ComponentsList* mComponentsList;
	};

}

#endif // ComponentIterators_h__