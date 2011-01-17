
void OnFlipperLeftButtonDown(Window@ wnd)
{
  EntityHandle flipper = gEntityMgr.FindFirstEntity("LeftFlipper");
  flipper.Set_bool("IsPressed", true);
}

void OnFlipperLeftButtonUp(Window@ wnd)
{
  EntityHandle flipper = gEntityMgr.FindFirstEntity("LeftFlipper");
  flipper.Set_bool("IsPressed", false);
}

void OnFlipperRightButtonDown(Window@ wnd)
{
  EntityHandle flipper = gEntityMgr.FindFirstEntity("RightFlipper");
  flipper.Set_bool("IsPressed", true);
}

void OnFlipperRightButtonUp(Window@ wnd)
{
  EntityHandle flipper = gEntityMgr.FindFirstEntity("RightFlipper");
  flipper.Set_bool("IsPressed", false);
}
