#include "Common.h"
#include "CmpPlatformVisual.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformVisual::Init( ComponentDescription& desc )
{

}

void EntitySystem::CmpPlatformVisual::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformVisual::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_DRAW_PLATFORM:
		Draw();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformVisual::RegisterReflection()
{

}

void EntitySystem::CmpPlatformVisual::Draw( void ) const
{
	DataContainer cont;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont);
	Vector2 pos;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
	float32 angle;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
	gGfxRenderer.DrawPolygonWithConversion((Vector2*)cont.GetData(), cont.GetSize(), pos, angle, GfxSystem::Color(0,170,0,170), GfxSystem::Pen(GfxSystem::Color(0,230,0)));
}