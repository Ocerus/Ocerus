void OnDraw(float32 delta)
{
	if (this.Get_uint32("FrameIndex") == this.Get_uint32("FrameCount") - 1)
	{
		gEntityMgr.DestroyEntity(this);
	}
}
