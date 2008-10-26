#ifndef ComponentIterators_h__
#define ComponentIterators_h__

#include <vector>
#include "../../Utility/Settings.h"

namespace EntitySystem 
{
	typedef std::vector<Component*> ComponentsList;

	class EntityComponentsIterator : public ComponentsList::const_iterator
	{
	public:
		EntityComponentsIterator(const ComponentsList& componentsList):
			mComponentsList(componentsList),
			ComponentsList::const_iterator(componentsList.begin())
		{
		}

		virtual ~EntityComponentsIterator() {}

		bool HasMore() { return operator!=(mComponentsList.end()); }
	private:
		const ComponentsList& mComponentsList;
	};
}

#endif // ComponentIterators_h__