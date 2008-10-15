#include "ComponentMgr.h"
#include "../Components/CmpPlatform.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	// register components
	mComponentCreationMethod[CT_PLATFORM] = CmpPlatform::CreateMe;
}