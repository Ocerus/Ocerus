#include "Common.h"
#include "CmpPlatform.h"

using namespace EntitySystem;

void CmpPlatform::Init(const ComponentDescription& desc)
{
	mNeco = 0;
}

void CmpPlatform::Deinit()
{

}

EntityMessage::eResult CmpPlatform::HandleMessage(const EntityMessage& msg)
{
	return EntityMessage::RESULT_OK;
}

void CmpPlatform::RegisterReflection()
{
	RegisterProperty<int32>("Neco", &GetNeco, &SetNeco, PROPACC_EDIT | PROPACC_SCRIPT);
}