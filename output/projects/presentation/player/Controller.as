#include "effects/Spawner.as"
#include "GameControl.as"
#include "player/GuiControl.as"

const uint32 STATE_NORMAL = 0;
const uint32 STATE_EXITING = 1;

void OnPostInit()
{
  this.RegisterDynamicProperty_bool("IsLight", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("ExplosionCooldown", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("JumpCooldown", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_Vector2("LastLandPosition", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("LastLandCollisionCooldown", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_uint32("State", PA_FULL_ACCESS, "");
  
  this.Set_float32("ExplosionCooldown", 0);
  this.Set_float32("JumpCooldown", 0);
  this.Set_float32("LastLandCollisionCooldown", 0.0f);
  this.Set_uint32("State", 0.0f);
  
  bool isLight = true;
  SetMode(this, isLight);
  SetPlayerState(STATE_NORMAL);
}

void SetPlayerState(uint32 state)
{
	this.Set_uint32("State", state);
}

uint32 GetPlayerState()
{
	return this.Get_uint32("State");
}

void OnUpdateLogic(float32 delta)
{
	if (GetPlayerState() != STATE_NORMAL) return;

	// rotate the cube
	float32 torque = 0;
	if (gInputMgr.IsKeyDown(KC_RIGHT)) torque = 1;
	if (gInputMgr.IsKeyDown(KC_LEFT)) torque = -1;
	
	if (this.Get_bool("IsLight")) torque *= 60;
	else torque *= 200;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
	
	// apply some additional linear force
	Vector2 force = Vector2(0, 0);
	if (gInputMgr.IsKeyDown(KC_RIGHT)) force = Vector2(1, 0);
	if (gInputMgr.IsKeyDown(KC_LEFT)) force = Vector2(-1, 0);
	
	if (this.Get_bool("IsLight")) force *= 10;
	else force *= 20;
	this.CallFunction("ApplyForce", PropertyFunctionParameters() << force);
	
	// reduce cooldowns
	this.Set_float32("ExplosionCooldown", MathUtils::Max(0.0f, this.Get_float32("ExplosionCooldown") - delta));
	this.Set_float32("JumpCooldown", MathUtils::Max(0.0f, this.Get_float32("JumpCooldown") - delta));

	// determine if we're on the ground or not
	const array_Vector2 contacts = this.Get_const_array_Vector2("Contacts");
	if (contacts.GetSize() > 0)
	{
		this.Set_float32("LastLandCollisionCooldown", JUMP_MAX_DELAY);
		Vector2 groundPos = Vector2(0,0);
		for (int i=0; i<contacts.GetSize(); ++i) groundPos += contacts[i];
		groundPos *= 1.0f / contacts.GetSize();
		this.Set_Vector2("LastLandPosition", groundPos);
	}
	else
	{
		this.Set_float32("LastLandCollisionCooldown", MathUtils::Max(0.0f, this.Get_float32("LastLandCollisionCooldown") - delta));
	}
}

void OnKeyPressed(eKeyCode key, uint32 char)
{
	if (GetPlayerState() != STATE_NORMAL) return;

	if (key == KC_DOWN)
	{
		SwitchMode(this);
	}
		
	if (key == KC_UP)
	{
		if (this.Get_bool("IsLight"))
		{
			TryJump(this);
		}
		else
		{
			TryExplode(this);
		}
	}
}

void OnCollisionStarted(EntityHandle other, Vector2 normal, Vector2 contactPoint)
{
	if (GetPlayerState() != STATE_NORMAL) return;

	if (other.GetTag() == 1)
	{
	  // finish sign
	  SetPlayerState(STATE_EXITING);
	}
	else if (other.GetTag() == 2)
	{
		// coin
		EntityHandle director = gEntityMgr.FindFirstEntity("Director");
		director.Set_uint32("Score", director.Get_uint32("Score") + 10);
		gEntityMgr.DestroyEntity(other);
		
		SpawnScore10(other.Get_Vector2("Position"));
	}
	else if (other.GetTag() == 5)
	{
		gEntityMgr.DestroyEntity(other);
		EntityHandle dest = gEntityMgr.FindFirstEntity("killme");
		gEntityMgr.DestroyEntity(dest);
	}
	else if (other.GetTag() == 6)
	{
		gEntityMgr.DestroyEntity(other);
		EntityHandle dest = gEntityMgr.FindFirstEntity("killme2");
		gEntityMgr.DestroyEntity(dest);
	}
}

void OnCollisionEnded(EntityHandle other)
{
	if (GetPlayerState() != STATE_NORMAL) return;
}

void OnDraw(float32 delta)
{
	if (GetPlayerState() == STATE_EXITING)
	{
		if (this.Get_Vector2("Scale").Length() < 0.1f)
		{
			ExitLevel();
		}
		else
		{
			this.Set_float32("Angle", this.Get_float32("Angle") + 0.4f);
			this.Set_Vector2("Scale", 0.9f * this.Get_Vector2("Scale"));
		}		
	}
	else
	{
  	// animate accordingly to the state
  	const float32 angleAnimSpeed = 15.0f;
  	const float32 PI = 3.14f;
  	float32 angle = this.Get_float32("YAngle");
  	if (this.Get_bool("IsLight") && angle > 0.0) angle -= angleAnimSpeed * delta;
  	if (!this.Get_bool("IsLight") && angle < PI) angle += angleAnimSpeed * delta;
  	if (angle < 0.0) angle = 0.0;
  	if (angle > PI) angle = PI;
  	this.Set_float32("YAngle", angle);
  }
}
