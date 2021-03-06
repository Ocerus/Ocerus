#include "Common.h"
#include "PhysicsDraw.h"

GfxSystem::PhysicsDraw::PhysicsDraw( void )
{
	uint32 flags = 0;
	flags += b2DebugDraw::e_shapeBit;
	flags += b2DebugDraw::e_jointBit;
	flags += b2DebugDraw::e_aabbBit;
	flags += b2DebugDraw::e_pairBit;
	flags += b2DebugDraw::e_centerOfMassBit;
	SetFlags(flags);
}

void GfxSystem::PhysicsDraw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& b2color )
{
	GfxSystem::Color color((uint8)(b2color.r*255), (uint8)(b2color.g*255), (uint8)(b2color.b*255), 255);
	gGfxRenderer.DrawPolygon(vertices, vertexCount, color, false);
}

void GfxSystem::PhysicsDraw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& b2color )
{
	GfxSystem::Color color((uint8)(b2color.r*255), (uint8)(b2color.g*255), (uint8)(b2color.b*255), 80);
	gGfxRenderer.DrawPolygon(vertices, vertexCount, color, true);
}

void GfxSystem::PhysicsDraw::DrawCircle( const b2Vec2& center, float32 radius, const b2Color& b2color )
{
	GfxSystem::Color color((uint8)(b2color.r*255), (uint8)(b2color.g*255), (uint8)(b2color.b*255), 255);
	gGfxRenderer.DrawCircle(center, radius, color, false);
}

void GfxSystem::PhysicsDraw::DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& b2color )
{
	OC_UNUSED(axis);
	GfxSystem::Color color((uint8)(b2color.r*255), (uint8)(b2color.g*255), (uint8)(b2color.b*255), 80);
	gGfxRenderer.DrawCircle(center, radius, color, true);
}

void GfxSystem::PhysicsDraw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& b2color )
{
	GfxSystem::Color color((uint8)(b2color.r*255), (uint8)(b2color.g*255), (uint8)(b2color.b*255), 255);
	Vector2 lineVertices[2];
	lineVertices[0] = p1;
	lineVertices[1] = p2;
	gGfxRenderer.DrawLine(lineVertices, color);
}

void GfxSystem::PhysicsDraw::DrawTransform( const b2Transform& xf )
{
	OC_UNUSED(xf);
}
