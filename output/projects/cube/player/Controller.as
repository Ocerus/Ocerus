void OnPostInit()
{
  this.RegisterDynamicProperty_bool("IsLight", PA_FULL_ACCESS, "");
}

void OnUpdateLogic(float32 delta)
{
	// rotate the cube
	float32 torque = 0;
	if (gInputMgr.IsKeyDown(KC_RIGHT)) torque = 1;
	if (gInputMgr.IsKeyDown(KC_LEFT)) torque = -1;
	
	if (this.Get_bool("IsLight")) torque *= 70;
	else torque *= 150;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
	
	// apply some additional linear force
	Vector2 force = Vector2(0, 0);
	if (gInputMgr.IsKeyDown(KC_RIGHT)) force = Vector2(1, 0);
	if (gInputMgr.IsKeyDown(KC_LEFT)) force = Vector2(-1, 0);
	
	if (this.Get_bool("IsLight")) force *= 10;
	else force *= 20;
	this.CallFunction("ApplyForce", PropertyFunctionParameters() << force);
}

void OnKeyPressed(eKeyCode key, uint32 char)
{
	if (key == KC_DOWN) SwitchState();	
	if (key == KC_UP)
	{
		if (this.Get_bool("IsLight"))
		{
		}
		else
		{
			
		}
	}
}

void SwitchState()
{
  this.Set_bool("IsLight", !this.Get_bool("IsLight"));
  
	if (this.Get_bool("IsLight"))
  {
		this.Set_float32("Density", 1.0f);
		this.Set_float32("Friction", 10.0f);  
  }
  else
  {
		this.Set_float32("Density", 10.0f);
		this.Set_float32("Friction", 0.1f);  
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
