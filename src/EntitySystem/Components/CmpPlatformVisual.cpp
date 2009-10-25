#include "Common.h"
#include "CmpPlatformVisual.h"
#include "DataContainer.h"

void EntityComponents::CmpPlatformVisual::Create( void )
{

}

void EntityComponents::CmpPlatformVisual::Destroy( void )
{

}

EntityMessage::eResult EntityComponents::CmpPlatformVisual::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::DRAW:
		Draw();
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::CmpPlatformVisual::RegisterReflection()
{
	RegisterFunction("MyFunction", &CmpPlatformVisual::MyFunction, PA_FULL_ACCESS);
}

void EntityComponents::CmpPlatformVisual::Draw( void ) const
{
	Vector2 pos = GetProperty("Position").GetValue<Vector2>();
	float32 angle = GetProperty("Angle").GetValue<float32>();
	EntityHandle blueprints = GetProperty("Blueprints").GetValue<EntityHandle>();
	GfxSystem::Color fillColor = blueprints.GetProperty("FillColor").GetValue<GfxSystem::Color>();
	bool flip = GetProperty("InitShapeFlip").GetValue<bool>();
	if (flip)
	{
		int32 col = fillColor.r + 30;
		fillColor.r = col > 255 ? 255 : (uint8)col;
		col = fillColor.g + 30;
		fillColor.g = col > 255 ? 255 : (uint8)col;
		col = fillColor.b + 30;
		fillColor.b = col > 255 ? 255 : (uint8)col;
	}
	gGfxRenderer.DrawPolygonWithConversion(GetProperty("Shape").GetValue<Vector2*>(), GetProperty("ShapeLength").GetValue<uint32>(), pos, angle, fillColor, GfxSystem::Pen(GfxSystem::Color(48, 30, 27, 200)));
}