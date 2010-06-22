void OnUpdateLogic(float32 delta)
{
	EntityHandle this = GetCurrentEntityHandle();
	float32 torque = 0;
	if (GetInputMgr().IsKeyDown(KC_RIGHT)) torque = 1;
	else if (GetInputMgr().IsKeyDown(KC_LEFT)) torque = -1;
	torque *= 30;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
}