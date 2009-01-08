#include "Common.h"
#include "CmpPlatformVisual.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformVisual::Init( void )
{

}

void EntitySystem::CmpPlatformVisual::Clean( void )
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
	PostMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont);
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
	float32 angle;
	PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
	PropertyHolder prop = GetProperty("Blueprints");
	EntityHandle blueprints = prop.GetValue<EntityHandle>();
	prop = blueprints.GetProperty("FillColor");
	GfxSystem::Color fillColor = prop.GetValue<GfxSystem::Color>();
	prop = GetProperty("InitShapeFlip");
	bool flip = prop.GetValue<bool>();
	if (flip)
	{
		/*int32 col = fillColor.r * 130/100;
		fillColor.r = col > 255 ? 255 : col;
		col = fillColor.g * 130/100;
		fillColor.g = col > 255 ? 255 : col;
		col = fillColor.b * 130/100;
		fillColor.b = col > 255 ? 255 : col;*/
		int32 col = fillColor.r + 30;
		fillColor.r = col > 255 ? 255 : col;
		col = fillColor.g + 30;
		fillColor.g = col > 255 ? 255 : col;
		col = fillColor.b + 30;
		fillColor.b = col > 255 ? 255 : col;
	}
	gGfxRenderer.DrawPolygonWithConversion((Vector2*)cont.GetData(), cont.GetSize(), pos, angle, fillColor, GfxSystem::Pen(GfxSystem::Color(48, 30, 27, 200)));
}