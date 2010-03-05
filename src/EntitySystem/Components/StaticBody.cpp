#include "Common.h"
#include "StaticBody.h"


void EntityComponents::StaticBody::Create( void )
{

}

void EntityComponents::StaticBody::Destroy( void )
{

}

EntityMessage::eResult EntityComponents::StaticBody::HandleMessage( const EntityMessage& msg )
{
	OC_UNUSED(msg);
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::StaticBody::RegisterReflection( void )
{

}
