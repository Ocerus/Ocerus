void OnPostInit()
{
  this.RegisterDynamicProperty_bool("IsLight", PA_FULL_ACCESS, "");
}

void OnUpdateLogic(float32 delta)
{
	// rotate the cube
	float32 torque = 0;
	if (inputMgr.IsKeyDown(KC_RIGHT)) torque = 1;
	if (inputMgr.IsKeyDown(KC_LEFT)) torque = -1;
	torque *= 70;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
	
	// apply some additional linear force
	Vector2 force = Vector2(0, 0);
	if (inputMgr.IsKeyDown(KC_RIGHT)) force = Vector2(1, 0);
	if (inputMgr.IsKeyDown(KC_LEFT)) force = Vector2(-1, 0);
	force = 10 * force;
	this.CallFunction("ApplyForce", PropertyFunctionParameters() << force);
}

void OnKeyPressed(eKeyCode key, uint32 char)
{
	if (key == KC_DOWN) SwitchState();		
}

void SwitchState()
{
  this.Set_bool("IsLight", !this.Get_bool("IsLight"));
}

void OnCollisionStarted(EntityHandle other, Vector2 normal, Vector2 contactPoint)
{

}

void OnCollisionEnded(EntityHandle other)
{

}

void OnDraw(float32 delta)
{
  // animate according to the state
  const float32 angleAnimSpeed = 10.0f;
  const float32 PI = 3.14f;
  float32 angle = this.Get_float32("YAngle");
  if (this.Get_bool("IsLight") && angle > 0.0) angle -= angleAnimSpeed * delta;
  if (!this.Get_bool("IsLight") && angle < PI) angle += angleAnimSpeed * delta;
  if (angle < 0.0) angle = 0.0;
  if (angle > PI) angle = PI;
  this.Set_float32("YAngle", angle);
}
