const float32 EXPLOSION_COOLDOWN = 1.0f;
const float32 JUMP_COOLDOWN = 2.0f;

void OnPostInit()
{
  this.RegisterDynamicProperty_bool("IsLight", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("ExplosionCooldown", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("JumpCooldown", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_Vector2("LastLandPosition", PA_FULL_ACCESS, "");
  this.RegisterDynamicProperty_float32("LastLandCollisionCooldown", PA_FULL_ACCESS, "");
  
  this.Set_float32("ExplosionCooldown", 0);
  this.Set_float32("JumpCooldown", 0);
  this.Set_float32("LastLandCollisionCooldown", 0.0f);
  
  bool isLight = true;
  SetState(isLight);
}

void SwitchState()
{
  SetState(!this.Get_bool("IsLight"));
}

void SetState(bool willBeLight)
{
  this.Set_bool("IsLight", willBeLight);
  
	if (this.Get_bool("IsLight"))
  {
		this.Set_float32("Density", 0.1f);
		this.Set_float32("Friction", 1.0f);  
  }
  else
  {
		this.Set_float32("Density", 10.0f);
		this.Set_float32("Friction", 0.1f);  
  }
}

void OnUpdateLogic(float32 delta)
{
	// rotate the cube
	float32 torque = 0;
	if (gInputMgr.IsKeyDown(KC_RIGHT)) torque = 1;
	if (gInputMgr.IsKeyDown(KC_LEFT)) torque = -1;
	
	if (this.Get_bool("IsLight")) torque *= 6;
	else torque *= 150;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
	
	// apply some additional linear force
	Vector2 force = Vector2(0, 0);
	if (gInputMgr.IsKeyDown(KC_RIGHT)) force = Vector2(1, 0);
	if (gInputMgr.IsKeyDown(KC_LEFT)) force = Vector2(-1, 0);
	
	if (this.Get_bool("IsLight")) force *= 1;
	else force *= 20;
	this.CallFunction("ApplyForce", PropertyFunctionParameters() << force);
	
	// reduce cooldowns
	this.Set_float32("ExplosionCooldown", MathUtils::Max(0.0f, this.Get_float32("ExplosionCooldown") - delta));
	this.Set_float32("JumpCooldown", MathUtils::Max(0.0f, this.Get_float32("JumpCooldown") - delta));

	// determine if we're on the ground or not
	const array_Vector2 contacts = this.Get_const_array_Vector2("Contacts");
	if (contacts.GetSize() > 0)
	{
		this.Set_float32("LastLandCollisionCooldown", 0.2f);
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
	if (key == KC_DOWN)
	{
		SwitchState();
	}
		
	if (key == KC_UP)
	{
		if (this.Get_bool("IsLight"))
		{
			if (this.Get_float32("JumpCooldown") > 0.0f || this.Get_float32("LastLandCollisionCooldown") == 0.0f) return;
			this.Set_float32("JumpCooldown", JUMP_COOLDOWN);
			
			Vector2 dir = this.Get_Vector2("Position") - this.Get_Vector2("LastLandPosition");
			dir.Normalize();
			this.CallFunction("ApplyLinearImpulse", PropertyFunctionParameters() << 0.8f * dir);
		}
		else
		{
			if (this.Get_float32("ExplosionCooldown") > 0) return;
			this.Set_float32("ExplosionCooldown", EXPLOSION_COOLDOWN);
			
			Vector2 myPos = this.Get_Vector2("Position");
			float32 radius = 1.0f;
			EntityPicker picker(myPos - Vector2(radius, radius), 1, 1);
			array_EntityHandle entities = picker.PickMultipleEntities(myPos + Vector2(radius, radius), 0);
			for (int i=0; i<entities.GetSize(); ++i)
			{
				Vector2 hisPos = entities[i].Get_Vector2("Position");
				Vector2 delta = hisPos - myPos;
				float32 length = MathUtils::Sqrt(delta.Normalize());
				Vector2 impulse = 10.0f * length * delta;
				entities[i].CallFunction("ApplyLinearImpulse", PropertyFunctionParameters() << impulse);
			}
		}
	}
}

void OnCollisionStarted(EntityHandle other, Vector2 normal, Vector2 contactPoint)
{
	if (other.GetTag() == 1)
	{
		Println("FINISH");
		gEntityMgr.DestroyEntity(this);
	}
}

void OnCollisionEnded(EntityHandle other)
{

}

void OnDraw(float32 delta)
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
