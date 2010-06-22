void OnUpdateLogic(float32 delta)
{
	// rotate the cube
	float32 torque = 0;
	if (GetInputMgr().IsKeyDown(KC_RIGHT)) torque = 1;
	if (GetInputMgr().IsKeyDown(KC_LEFT)) torque = -1;
	torque *= 70;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
	
	// apply some additional linear force
	Vector2 force = Vector2(0, 0);
	if (GetInputMgr().IsKeyDown(KC_RIGHT)) force = Vector2(1, 0);
	if (GetInputMgr().IsKeyDown(KC_LEFT)) force = Vector2(-1, 0);
	force = 10 * force;
	this.CallFunction("ApplyForce", PropertyFunctionParameters() << force);
}