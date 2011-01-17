
const float32 MAX_ANGLE_DELTA = 0.6f;
const float32 ANGLE_IMPULSE_RATIO = 10000.0f;
const float32 ANGLE_CHANGE_RATIO = 10.0f;



void OnPostInit()
{
  // register properties so that we can set them up in the editor... these are visible
  this.RegisterDynamicProperty_bool("LeftSided", PA_FULL_ACCESS, "True if the flipper is on the left side.");
  this.RegisterDynamicProperty_Vector2("Pivot", PA_FULL_ACCESS, "Location of the pivot point in local coords.");

  // these are internal variables
  this.RegisterDynamicProperty_float32("InitAngle", PA_SCRIPT_READ | PA_SCRIPT_WRITE, "");
  this.RegisterDynamicProperty_Vector2("InitPivotPosition", PA_SCRIPT_READ | PA_SCRIPT_WRITE, "");
  this.RegisterDynamicProperty_bool("IsPressed", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");
  // set initial values when the script is loaded
  Vector2 pivotWorldPos = this.Get_Vector2("Position") + MathUtils::RotateVector(this.Get_Vector2("Pivot"), this.Get_float32("Angle"));
  this.Set_Vector2("InitPivotPosition", pivotWorldPos);
  this.Set_float32("InitAngle", this.Get_float32("Angle"));
}

void OnUpdateLogic(float32 delta)
{
  // get current values
  float32 angle = this.Get_float32("Angle");
  float32 initAngle = this.Get_float32("InitAngle");


  // react on the keys and (indirectly) change the angle by applying an impulse to the object
  float32 impulse = ANGLE_IMPULSE_RATIO * delta;
  float32 change = ANGLE_CHANGE_RATIO * delta;
  if (this.Get_bool("LeftSided"))
  {
    //if (gInputMgr.IsKeyDown(KC_LEFT))
    if (gInputMgr.IsKeyDown(KC_LEFT) || this.Get_bool("IsPressed"))
    {
      // not entirely in the upper position yet
      if (angle > initAngle - MAX_ANGLE_DELTA) this.CallFunction("ApplyAngularImpulse", PropertyFunctionParameters() << -impulse);
    }
    else
    {
      // move the flipper down
      this.Set_float32("Angle", this.Get_float32("Angle") + change);
      this.CallFunction("ZeroAngularVelocity", PropertyFunctionParameters());
    }      
  }
  else
  {

    //if (gInputMgr.IsKeyDown(KC_RIGHT))
    if (gInputMgr.IsKeyDown(KC_RIGHT) || this.Get_bool("IsPressed"))
    {
      // not entirely in the upper position yet
      if (angle < initAngle + MAX_ANGLE_DELTA) this.CallFunction("ApplyAngularImpulse", PropertyFunctionParameters() << impulse);
    }
    else
    {
      // move the flipper down
      this.Set_float32("Angle", this.Get_float32("Angle") - change);
      this.CallFunction("ZeroAngularVelocity", PropertyFunctionParameters());
    }
  }
}

void OnUpdatePostPhysics(float32 delta)
{
  EnsurePaddleIsInBounds();
}

void EnsurePaddleIsInBounds()
{
  // eliminate eny unwanted velocities accumulating in our body
  this.CallFunction("ZeroLinearVelocity", PropertyFunctionParameters());  

  // get current values
  float32 angle = this.Get_float32("Angle");
  float32 initAngle = this.Get_float32("InitAngle");
  
  // make sure the angle stays in the bounds
  if (this.Get_bool("LeftSided"))
  {
    if (angle > initAngle) angle = initAngle;
    if (angle < initAngle - MAX_ANGLE_DELTA) angle = initAngle - MAX_ANGLE_DELTA;
  }
  else
  {
    if (angle < initAngle) angle = initAngle;
    if (angle > initAngle + MAX_ANGLE_DELTA) angle = initAngle + MAX_ANGLE_DELTA;
  }
  
  // set new values to the object
  if (angle != this.Get_float32("Angle"))
  {
    this.Set_float32("Angle", angle);
    this.CallFunction("ZeroVelocity", PropertyFunctionParameters());  
  }
  Vector2 pivotLocalPos = MathUtils::RotateVector(this.Get_Vector2("Pivot"), angle);
  Vector2 newPos = this.Get_Vector2("InitPivotPosition") - pivotLocalPos;
  this.Set_Vector2("Position", newPos);
}

