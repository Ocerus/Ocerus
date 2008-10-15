#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

namespace EntitySystem
{
	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:



		void LoadResourcesForActiveComponents(); // calls LoadResource of all instantiated components
		void ReleaseResourcesForAllComponents();
	}
}

#endif