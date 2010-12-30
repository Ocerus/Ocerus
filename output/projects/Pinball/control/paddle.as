const float32 MAX_ANGLE_DELTA = 0.6;
const float32 ANGLE_CHANGE_RATIO = 10.0;


void OnPostInit()
{
  // register properties so that we can set them up in the editor... these are visible
  this.RegisterDynamicProperty_bool("LeftSided", PA_FULL_ACCESS, "True if the paddle is on the left side.");
  this.RegisterDynamicProperty_Vector2("Pivot", PA_FULL_ACCESS, "Location of the pivot point in local coords.");

  // these are internal variable; they're read-only in the editor
  this.RegisterDynamicProperty_float32("InitAngle", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");
  this.RegisterDynamicProperty_Vector2("InitPivotPosition", PA_SCRIPT_READ | PA_SCRIPT_WRITE | PA_EDIT_READ, "");

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
  
  // compute new rotation of the paddle and react to the keys
  if (this.Get_bool("LeftSided"))
  {
    if (angle > initAngle) angle = initAngle;
    if (gInputMgr.IsKeyDown(KC_LEFT)) angle -= ANGLE_CHANGE_RATIO * delta;
    else angle += ANGLE_CHANGE_RATIO * delta;
    if (angle < initAngle - MAX_ANGLE_DELTA) angle = initAngle - MAX_ANGLE_DELTA;
  }
  else
  {
    if (angle < initAngle) angle = initAngle;
    if (gInputMgr.IsKeyDown(KC_RIGHT)) angle += ANGLE_CHANGE_RATIO * delta;
    else  angle -= ANGLE_CHANGE_RATIO * delta;
    if (angle > initAngle + MAX_ANGLE_DELTA) angle = initAngle + MAX_ANGLE_DELTA;
  }
  
  // set new values to the object
  this.Set_float32("Angle", angle);
  Vector2 pivotLocalPos = MathUtils::RotateVector(this.Get_Vector2("Pivot"), angle);
  Vector2 newPos = this.Get_Vector2("InitPivotPosition") - pivotLocalPos;
  this.Set_Vector2("Position", newPos);
  
  // eliminate eny unwanted velocities accumulating in our body
  this.CallFunction("ZeroVelocity", PropertyFunctionParameters());  
}
