#include "effects/Spawner.as"

const float32 EXPLOSION_COOLDOWN = 1.0f;
const float32 JUMP_COOLDOWN = 2.0f;
const float32 EXPLOSION_RATIO = 20.0f;
const float32 EXPLOSION_PULL_RADIUS = 1.0f;
const float32 EXPLOSION_DESTROY_RADIUS = 1.5f;
const uint32 EXPLOSION_DESTROY_COUNT = 5;
const float32 JUMP_RATIO = 10.0f;
const float32 JUMP_MAX_DELAY = 0.2f;


bool CanJump(EntityHandle entity)
{
	return (entity.Get_bool("IsLight") && entity.Get_float32("JumpCooldown") <= 0.0f && entity.Get_float32("LastLandCollisionCooldown") > 0.0f);
}

bool CanExplode(EntityHandle entity)
{
	return (!entity.Get_bool("IsLight") && entity.Get_float32("ExplosionCooldown") <= 0.0f);
}

void TryJump(EntityHandle entity)
{
	if (!CanJump(entity)) return;
	entity.Set_float32("JumpCooldown", JUMP_COOLDOWN);
			
	Vector2 dir = entity.Get_Vector2("Position") - entity.Get_Vector2("LastLandPosition");
	dir.Normalize();
	entity.CallFunction("ApplyLinearImpulse", PropertyFunctionParameters() << JUMP_RATIO * dir);
			
	Vector2 dustPos = 0.5f * (entity.Get_Vector2("Position") + entity.Get_Vector2("LastLandPosition"));
	SpawnJumpDust(dustPos);
}

void TryExplode(EntityHandle entity)
{
	if (!CanExplode(entity)) return;
	entity.Set_float32("ExplosionCooldown", EXPLOSION_COOLDOWN);
			
	Vector2 myPos = entity.Get_Vector2("Position");
	EntityPicker picker(myPos - Vector2(EXPLOSION_PULL_RADIUS, EXPLOSION_PULL_RADIUS), 1, 1);
	EntityHandle[] entities;
	picker.PickMultipleEntities(entities, myPos + Vector2(EXPLOSION_PULL_RADIUS, EXPLOSION_PULL_RADIUS), 0);
	uint32 destroyedCount = 0;
	for (int i=0; i< int(entities.length()); ++i)
	{
		if (entities[i] == entity) continue;
				
		Vector2 hisPos = entities[i].Get_Vector2("Position");
		Vector2 delta = hisPos - myPos;
		float32 length = delta.Normalize();
		if (destroyedCount < EXPLOSION_DESTROY_COUNT && length <= EXPLOSION_DESTROY_RADIUS) 
		{
			SpawnExplosion(hisPos);
					
			Vector2 hisScale = entities[i].Get_Vector2("Scale");
			Vector2 impulse =  EXPLOSION_RATIO / 10.0f * MathUtils::Sqrt(length) * delta;
			if (hisScale.x * hisScale.y > 1)
			{
				SpawnSmallStone(hisPos + Vector2(0.3f, 0.3f), Vector2(0.7f,0.5f), impulse + Vector2(0.5f,0.5f));
				SpawnSmallStone(hisPos + Vector2(-0.3f, 0.4f), Vector2(0.5f,0.6f), impulse + Vector2(-0.5f,0.5f));
				SpawnSmallStone(hisPos + Vector2(-0.0f, -0.3f), Vector2(0.8f,0.7f), impulse + Vector2(0.0f,-0.5f));
			}
					
			gEntityMgr.DestroyEntity(entities[i]);
			destroyedCount++;
		}
		else
		{
			Vector2 impulse = EXPLOSION_RATIO * MathUtils::Sqrt(length) * delta;
			entities[i].CallFunction("ApplyLinearImpulse", PropertyFunctionParameters() << impulse);
		}
	}
			
	SpawnExplosionDust(entity.Get_Vector2("Position"));
}

void SwitchMode(EntityHandle entity)
{
  SetMode(entity, !entity.Get_bool("IsLight"));
}

void SetMode(EntityHandle entity, bool willBeLight)
{
  entity.Set_bool("IsLight", willBeLight);
  
	if (entity.Get_bool("IsLight"))
  {
		entity.Set_float32("Density", 1.0f);
		entity.Set_float32("Friction", 1.0f);  
  }
  else
  {
		entity.Set_float32("Density", 15.0f);
		entity.Set_float32("Friction", 0.3f);  
  }
}

