#include "Common.h"
#include "CmpProjectile.h"
#include "../../Core/Game.h"
#include "Box2D.h"

using namespace EntitySystem;

#define RADIUS_RATIO 0.001f
#define DAMAGE_RATIO 100.0f
#define KNOCKBACK_RATIO 10.0f

void EntitySystem::CmpProjectile::Init( void )
{
	mBlueprints.Invalidate();
	mInitBodyPosition = Vector2_Zero;
	mInitBodySpeed = Vector2_Zero;
	mMaxDistance = 0;

	mTrailPS.SetNull();
	mBody = 0;
}

void EntitySystem::CmpProjectile::Clean( void )
{
	if (mBody)
	{
		gApp.GetCurrentGame()->GetPhysics()->DestroyBody(mBody);
		mBody = 0;
	}
	if (!mTrailPS.IsNull())
	{
		gPSMgr.UnregisterPS(mTrailPS);
		mTrailPS.SetNull();
	}
}

EntityMessage::eResult EntitySystem::CmpProjectile::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		PostInit();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_UPDATE_PHYSICS_SERVER:
		if (mBody)
		{
			float32 distSq = MathUtils::DistanceSquared(mBody->GetPosition(), mInitBodyPosition); 
			if (distSq > MathUtils::Sqr(mMaxDistance))
				Die(true);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_PROJECTILE:
		Draw();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_STRIKE:
		BS_DASSERT(msg.data);
		Strike(*(EntityHandle*)msg.data);
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpProjectile::RegisterReflection( void )
{
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodyPosition", &GetInitBodyPosition, &SetInitBodyPosition, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodySpeed", &GetInitBodySpeed, &SetInitBodySpeed, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("MaxDistance", &GetMaxDistance, &SetMaxDistance, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpProjectile::Draw( void ) const
{
	if (!mBody)
		return;

	PropertyHolder prop = mBlueprints.GetProperty("Size");
	float32 radius = RADIUS_RATIO * prop.GetValue<uint32>();

	// draw the particle effect
	if (!mTrailPS.IsNull())
	{
		mTrailPS->MoveTo(mBody->GetPosition(), false);
		prop = mBlueprints.GetProperty("TrailEffectScale");
		mTrailPS->SetScale(radius * prop.GetValue<float32>());
		mTrailPS->Fire();
		mTrailPS->Render();
	}
	
	// draw the projectile
	GfxSystem::Color color(150,150,150,180);
	Vector2 square[] = {Vector2(-radius,-radius), Vector2(radius,-radius), Vector2(radius,radius), Vector2(-radius,radius)};
	gGfxRenderer.DrawPolygonWithConversion(square, 4, mBody->GetPosition(), MathUtils::Random(0, MathUtils::TWO_PI), color);
}

void EntitySystem::CmpProjectile::PostInit( )
{
	PropertyHolder prop;

	// create body
	b2BodyDef bodyDef;
	bodyDef.position = mInitBodyPosition;
	bodyDef.userData = GetOwnerPtr();
	bodyDef.isBullet = true;
	mBody = gApp.GetCurrentGame()->GetPhysics()->CreateBody(&bodyDef);

	// create shape
	b2CircleDef shapeDef;
	prop = mBlueprints.GetProperty("Material");
	EntityHandle material = prop.GetValue<EntityHandle>();
	prop = material.GetProperty("Density");
	float32 density = prop.GetValue<float32>();
	shapeDef.density = density;
	prop = mBlueprints.GetProperty("Size");
	shapeDef.radius = RADIUS_RATIO * prop.GetValue<uint32>();
	shapeDef.isSensor = true;
	shapeDef.filter.categoryBits = (1 << 15) | (1 << 14); // 15 means AA projectiles, 14 means normal projectile
	shapeDef.filter.maskBits = 16383; // colide with everything except projectiles
	shapeDef.userData = GetOwnerPtr();
	mBody->CreateShape(&shapeDef);

	// compute mass
	mBody->SetMassFromShapes();

	// set the firing speed
	mBody->SetLinearVelocity(mInitBodySpeed);

	// create the PS effect
	prop = mBlueprints.GetProperty("TrailEffect");
	StringKey effect = prop.GetValue<StringKey>();
	prop = mBlueprints.GetProperty("ResourceGroup");
	StringKey resGroup = prop.GetValue<StringKey>();
	mTrailPS = gPSMgr.SpawnPS(resGroup, effect);
	if (!mTrailPS.IsNull())
		mTrailPS->MoveTo(mBody->GetPosition());
}

void EntitySystem::CmpProjectile::Strike( EntityHandle target )
{
	BS_ASSERT(target.IsValid());
	PropertyHolder prop;
	//TODO kdyz to nebude platforma, tak knockback udelat na parent (coz musi bejt platforma)
	if (target.GetType() == ET_PLATFORM)
	{
		b2Body* targetBody;
		target.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &targetBody);
		//TODO Strike by se vubec nemel zavolat, pokud targetBody je null.
		//TODO Tohle muze nastat, pokud dve strely v jeden frame trefi platformu a prvni z nich ji zabije.
		if (targetBody)
		{
			prop = target.GetProperty("AbsolutePosition");
			Vector2 targetPos = prop.GetValue<Vector2>();
			Vector2 forceDir = mBody->GetLinearVelocity();
			forceDir.Normalize();
			prop = mBlueprints.GetProperty("KnockbackRatio");
			float32 knockback = prop.GetValue<float32>();
			targetBody->ApplyForce(KNOCKBACK_RATIO * knockback * forceDir, mBody->GetPosition());
			target.PostMessage(EntityMessage::TYPE_KNOCKBACK_DETACH, &knockback);
		}
		else
		{
			targetBody = 0; // tohle je jen zachytnej bod pro breakpoint :)
		}
	}
	prop = mBlueprints.GetProperty("PowerRatio");
	float32 damage = DAMAGE_RATIO * prop.GetValue<float32>();
	target.PostMessage(EntityMessage::TYPE_DAMAGE, &damage);

	Die(false);
}

void EntitySystem::CmpProjectile::Die( const bool splash )
{
	// create the PS explode/splash effect
	PropertyHolder prop;
	if (splash)
		prop = mBlueprints.GetProperty("SplashEffect");
	else
		prop = mBlueprints.GetProperty("ExplodeEffect");
	StringKey effect = prop.GetValue<StringKey>();
	prop = mBlueprints.GetProperty("ResourceGroup");
	StringKey resGroup = prop.GetValue<StringKey>();
	GfxSystem::ParticleSystemPtr ps = gPSMgr.SpawnPS(resGroup, effect);
	if (!ps.IsNull())
	{
		if (splash)
			prop = mBlueprints.GetProperty("SplashEffectScale");
		else
			prop = mBlueprints.GetProperty("ExplodeEffectScale");
		ps->SetScale(prop.GetValue<float32>());
		ps->MoveTo(mBody->GetPosition());
		ps->Fire();
	}

	// delete the entity
	gEntityMgr.DestroyEntity(GetOwner());
}