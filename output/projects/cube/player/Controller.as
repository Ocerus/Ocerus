bool leftPressed = false;
bool rightPressed = false;
const eKeyCode LEFT = 0xCB;
const eKeyCode RIGHT = 0xCD;
const eKeyCode UP = 0xC8;


void OnUpdateLogic(float32 delta)
{
	EntityHandle this = GetCurrentEntityHandle();
	float32 torque = 0;
	if (rightPressed) torque = 1;
	if (leftPressed) torque = -1;
	torque *= 30;
	this.CallFunction("ApplyTorque", PropertyFunctionParameters() << torque);
}


void OnKeyPressed(eKeyCode key, uint32 char)
{
	if (key == LEFT) leftPressed = true;
	if (key == RIGHT) rightPressed = true;
}


void OnKeyReleased(eKeyCode key, uint32 char)
{
	leftPressed = false;
	rightPressed = false;
}
