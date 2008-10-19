#ifndef _COMPONENTITERATORS_H_
#define _COMPONENTITERATORS_H_

#include "../EntityMgr/EntityHandle.h"

namespace EntitySystem 
{
	class EntityComponentsIterator
	{
	public:
		EntityComponentsIterator(EntityHandle h);
		EntityComponentsIterator& operator++();
		EntityComponentsIterator operator++(int);
		inline Component operator*() { return GetComponent(); }

		bool HasMore() const;
		Component* GetComponent() const;

	private:
		void SetToFirstValidComponent();

		EntityID mEntityID;
		int32 mIndex;
	};
}

#endif