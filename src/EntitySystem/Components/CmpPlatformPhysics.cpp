#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformPhysics::Init( ComponentDescription& desc )
{

}

void EntitySystem::CmpPlatformPhysics::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformPhysics::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformPhysics::RegisterReflection()
{
	RegisterProperty<Vector2&>("Position", &GetPosition, &SetPosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

Vector2& EntitySystem::CmpPlatformPhysics::GetPosition( void ) const
{
	return const_cast<Vector2&>(mBody->GetPosition());
}

void EntitySystem::CmpPlatformPhysics::SetPosition( Vector2& pos )
{

}