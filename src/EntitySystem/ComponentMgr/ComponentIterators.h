#ifndef _COMPONENTITERATORS_H_
#define _COMPONENTITERATORS_H_

#include "../EntityMgr/EntityHandle.h"

namespace EntitySystem 
{
	class EntityComponentsIterator
	{
	public:
		EntityComponentsIterator(EntityHandle h);
		EntityComponentsIterator& operator++(void);
		EntityComponentsIterator operator++(int);
		inline Component operator*() { return GetComponent(); }

		bool HasMore(void) const;
		Component* GetComponent(void) const;

	private:
		void SetToFirstValidComponent(void);

		EntityID mEntityID;
		int32 mIndex;
	};
}

#endif