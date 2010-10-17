void TextboxSetText(string text)
{
	GetWindow("TextBox/Text").SetText(text);
}

void OnShown(Window@ wnd)
{
}

void OnCloseClicked(Window@ wnd)
{
	this.Set_bool("Visible", false);
	gEntityMgr.FindFirstEntity("MenuGUI").Set_bool("Enabled", true);
}