#include "Common.h"
#include "SimpleAI.h"
#include "AIMgr.h"
#include "../EntitySystem/EntityMgr/EntityEnums.h"
#include "../Utility/Properties/PropertyHolder.h"
#include "../EntitySystem/Components/CmpEngineParams.h"

namespace AISystem {
	template <class T> class is_prop_not_valid : public std::unary_function<T, bool> 
	{
	public:
		bool operator( ) ( T& val ) 
		{
			return !val.IsValid();
		}
	};

	template <class T> class is_nested_prop_not_valid : public std::unary_function<T, bool> 
	{
	public:
		bool operator( ) ( T& val ) 
		{
			return !val.mHandle.IsValid();
		}
	};

	void RefreshEntitiesIn( std::list<TurretInfo> & list ) {
		list.remove_if( is_nested_prop_not_valid<TurretInfo>() );
	}
	
	void RefreshEntitiesIn( std::list<EntitySystem::EntityHandle> & list ) {
		list.remove_if( is_prop_not_valid<EntitySystem::EntityHandle>() );
	}

	void LoadEntitiesOfType( string name_prefix, string name_suffix, uint32 count,
			std::list<TurretInfo> & target ) {
		for (uint32 i = 1; i <= count; ++i) {
			target.push_back( 
				TurretInfo(gEntityMgr.FindFirstEntity( name_prefix + StringConverter::ToString(i) + name_suffix ))
			);
		}
	}

	void LoadEntitiesOfType( string name_prefix, string name_suffix, uint32 count,
			std::list<EntitySystem::EntityHandle> & target )
	{
		for (uint32 i = 1; i <= count; ++i) {
			target.push_back( 
				gEntityMgr.FindFirstEntity( name_prefix + StringConverter::ToString(i) + name_suffix )
			);
		}
	}

	void SimpleAI::DoLogic( float32 delta ) {
		Refresh();
		SetEngines();
		//TargetWeapons();
	}

	void SimpleAI::SetEngines() {
		PropertyHolder prop = mEnemyPlayer.GetProperty( "AbsolutePosition" );
		Vector2 enemy_location = prop.GetValue<Vector2&>();
		prop = mPawn.GetProperty( "AbsolutePosition" );
		Vector2 my_location = prop.GetValue<Vector2&>();
		Vector2 relative_enemy_location = enemy_location - my_location;
		prop = mPawn.GetProperty( "Angle" );
		Vector2 my_orientation = MathUtils::VectorFromAngle(prop.GetValue<float32>());
		
		Vector2 intended_move_vector( -relative_enemy_location.y, relative_enemy_location.x );

		intended_move_vector.Normalize();
		float32 dot = MathUtils::Dot(my_orientation, intended_move_vector);

		if ( dot < 0 )
			intended_move_vector = -intended_move_vector;

		// too little change to consider
		if ( 1.05f > dot && 0.95f < dot ) return;

		Vector2 ideal_engine_direction = -intended_move_vector;

		std::list<TurretInfo>::iterator iter = mEngines.begin();
		while ( iter != mEngines.end() ) {
			prop = iter->mHandle.GetProperty( "AbsoluteAngle" );			
			float32 engine_absolute_current = prop.GetValue<float32>();
			engine_absolute_current = MathUtils::Wrap( engine_absolute_current, -MathUtils::PI, MathUtils::PI );
			prop = iter->mHandle.GetProperty( "AbsoluteDefaultAngle" );
			float32 engine_absolute_default = prop.GetValue<float32>();
			engine_absolute_default = MathUtils::Wrap( engine_absolute_default, -MathUtils::PI, MathUtils::PI );

			float32 required_change = MathUtils::Angle(ideal_engine_direction) -
				engine_absolute_current;
			required_change = MathUtils::Wrap( required_change, -MathUtils::PI, MathUtils::PI );
			
			float32 possible_change = 
				MathUtils::Clamp( required_change, -iter->mHalfArcAngle,
					iter->mHalfArcAngle );

			float32 power;
			if ( possible_change == required_change ) {
				power = 1;
			} else {
				power = MathUtils::Clamp( 1 - MathUtils::AngleDistance( possible_change, required_change )*1.5f, 0, 1 );
			}
			iter->mHandle.PostMessage( EntitySystem::EntityMessage::TYPE_SET_POWER_RATIO, &power );
			if (power > 0) {
				iter->mCurrentRelativeAngle = possible_change;
				//iter->mHandle.PostMessage( EntitySystem::EntityMessage::TYPE_SET_RELATIVE_ANGLE,
					//&( iter->mCurrentRelativeAngle ) );
			}

			++iter;
		}
	}

	void SimpleAI::TargetWeapons() {		
		std::list<TurretInfo>::iterator iter = mWeapons.begin();
		while ( iter != mWeapons.end() ) {
			PropertyHolder prop = iter->mHandle.GetProperty( "AbsolutePosition" );
			Vector2 weapon_absolute = prop.GetValue<Vector2>();
			prop = iter->mHandle.GetProperty( "AbsoluteDefaultAngle" );
			float32 weapon_absolute_default_angle = prop.GetValue<float32>();
			bool HasTarget = false;
			std::list<EntitySystem::EntityHandle>::iterator platform_iterator = mEnemyPlatforms.begin();
			
			while ( platform_iterator != mEnemyPlatforms.end() ) {
				prop = platform_iterator->GetProperty( "AbsolutePosition" );
				Vector2 platform_absolute = prop.GetValue<Vector2>();
				Vector2 platform_relative = platform_absolute - weapon_absolute;
				
				float32 required_weapon_angle = MathUtils::Angle( platform_relative ) - weapon_absolute_default_angle ;
				if ( MathUtils::IsAngleInRange( required_weapon_angle, -iter->mHalfArcAngle, iter->mHalfArcAngle ) ) {
					HasTarget = true;
					iter->mHandle.PostMessage( EntitySystem::EntityMessage::TYPE_SET_RELATIVE_ANGLE,
						&required_weapon_angle);
					iter->mHandle.PostMessage( EntitySystem::EntityMessage::TYPE_START_SHOOTING );
					break;
				}
				++platform_iterator;
			}
			if ( HasTarget == false )
				iter->mHandle.PostMessage( EntitySystem::EntityMessage::TYPE_STOP_SHOOTING );
			++iter;
		}
	}

	void SimpleAI::Refresh() {
		RefreshEntitiesIn( mEngines );
		RefreshEntitiesIn( mWeapons );
		RefreshEntitiesIn( mEnemyPlatforms );
	}

	SimpleAI::SimpleAI( EntitySystem::EntityHandle my_player,
			EntitySystem::EntityHandle enemy_player ) : GenericAI(my_player), mEnemyPlayer(enemy_player)
	{
		LoadEntitiesOfType( "engine", "-1", 8, mEngines );
		LoadEntitiesOfType( "weapon", "-1", 12, mWeapons );
		LoadEntitiesOfType( "platform", "", 23, mEnemyPlatforms );
	}

	TurretInfo::TurretInfo( EntitySystem::EntityHandle handle ) : mHandle(handle) {
		PropertyHolder prop = handle.GetProperty( "Blueprints" );
		prop = prop.GetValue<EntitySystem::EntityHandle>().GetProperty( "ArcAngle" );
		mHalfArcAngle = prop.GetValue<float32>() / 2;
		prop = handle.GetProperty( "RelativeAngle" );
		mCurrentRelativeAngle = prop.GetValue<float32>();
		mPossibleLeft = mHalfArcAngle;
	}
}