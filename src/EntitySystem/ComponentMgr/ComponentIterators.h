#ifndef ComponentIterators_h__
#define ComponentIterators_h__

#include <vector>
#include "../../Utility/Settings.h"

namespace EntitySystem 
{

	/// Forward declarations
	class Component;
	
	typedef std::vector<Component*> ComponentsList;

	/** Enables iterating over a collection of components. Wraps around an STL iterator.
	*/
	class EntityComponentsIterator : public ComponentsList::const_iterator
	{
	public:
		EntityComponentsIterator(const ComponentsList& componentsList):
			mComponentsList(componentsList),
			ComponentsList::const_iterator(componentsList.begin())
		{
		}

		virtual ~EntityComponentsIterator(void) {}

		/// Use this method as a loop condition instead of comparing the iterator to a collection end().
		bool HasMore(void) { return operator!=(mComponentsList.end()); }
	private:
		const ComponentsList& mComponentsList;
	};
}

#endif // ComponentIterators_h__