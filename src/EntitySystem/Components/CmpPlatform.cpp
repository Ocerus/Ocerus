#include "Common.h"
#include "CmpPlatform.h"

using namespace EntitySystem;

void CmpPlatform::Init(const ComponentDescription& desc) 
{

}

void CmpPlatform::Deinit()
{

}

EntityMessage::eResult CmpPlatform::HandleMessage(const EntityMessage& msg)
{
	return EntityMessage::RESULT_OK;
}