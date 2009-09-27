#include "Common.h"
#include "EntityPicker.h"

using namespace EntitySystem;

uint8* EntityPicker::msPriorities = 0;

void EntitySystem::EntityPicker::SetupPriorities( void )
{
	BS_ASSERT_MSG(!msPriorities, "Priorities are already set up");
	msPriorities = DYN_NEW uint8[NUM_ENTITY_TYPES];
	for (int i=0; i<NUM_ENTITY_TYPES; ++i)
		msPriorities[i] = 0;
	msPriorities[ET_PLATFORM] = 50;
	msPriorities[ET_ENGINE] = 70;
	msPriorities[ET_WEAPON] = 80;
}

void EntitySystem::EntityPicker::CleanPriorities( void )
{
	if (msPriorities)
		DYN_DELETE_ARRAY msPriorities;
}

EntitySystem::EntityPicker::EntityPicker( const Vector2& worldCursorPos ): 
	mResult(), 
	mResultType(ET_UNKNOWN),
	mCursorWorldPosition(worldCursorPos) 
{

}

EntitySystem::EntityPicker::EntityPicker( const int32 screenCursorX, const int32 screenCursorY ):
	mResult(),
	mResultType(ET_UNKNOWN)
{
	mCursorWorldPosition.x = gGfxRenderer.ScreenToWorldX(screenCursorX);
	mCursorWorldPosition.y = gGfxRenderer.ScreenToWorldY(screenCursorY);
}

void EntitySystem::EntityPicker::Update( const EntityHandle entity, const Vector2& circleCenter, const float32 circleRadius )
{
	if (MathUtils::DistanceSquared(circleCenter, mCursorWorldPosition) <= MathUtils::Sqr(circleRadius))
		PickIfHigherPriority(entity);
}

void EntitySystem::EntityPicker::Update( const EntityHandle entity, const float32 rectX1, const float32 rectY1, const float32 rectX2, const float32 rectY2 )
{
	if (mCursorWorldPosition.x>=rectX1 && mCursorWorldPosition.y>=rectY1
		&& mCursorWorldPosition.x<=rectX2 && mCursorWorldPosition.y<=rectY2)
		PickIfHigherPriority(entity);
}

EntitySystem::EntityHandle EntitySystem::EntityPicker::GetResult( void ) const
{
	return mResult;
}

void EntitySystem::EntityPicker::PickIfHigherPriority( const EntityHandle entity )
{
	eEntityType type = entity.GetType();
	if (msPriorities[type] > msPriorities[mResultType])
	{
		mResult = entity;
		mResultType = type;
	}
	//TODO jeste bysme v else vetvi mohli zkontrlovat, pokud se priority rovnaji, jestli tenhle pick neni lepsi, tj.
	//	jestli neni bliz kursoru stred kruhu nebo obdelnika (tzn. musime si pak pamatovat posledni tuto vzdalenost)
}

