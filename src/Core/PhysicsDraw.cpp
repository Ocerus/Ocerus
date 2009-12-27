#include "Common.h"
#include "PhysicsDraw.h"

Core::PhysicsDraw::PhysicsDraw( void )
{
	uint32 flags = 0;
	flags += b2DebugDraw::e_shapeBit;
	flags += b2DebugDraw::e_jointBit;
	flags += b2DebugDraw::e_coreShapeBit;
	flags += b2DebugDraw::e_aabbBit;
	flags += b2DebugDraw::e_obbBit;
	flags += b2DebugDraw::e_pairBit;
	flags += b2DebugDraw::e_centerOfMassBit;
	SetFlags(flags);
}

void Core::PhysicsDraw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{

}

void Core::PhysicsDraw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const b2Color& color )
{

}

void Core::PhysicsDraw::DrawCircle( const b2Vec2& center, float32 radius, const b2Color& color )
{

}

void Core::PhysicsDraw::DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color )
{

}

void Core::PhysicsDraw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const b2Color& color )
{

}

void Core::PhysicsDraw::DrawXForm( const b2XForm& xf )
{

}
