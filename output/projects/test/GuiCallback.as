void PrintWindowName(Window@ window)
{
  Println("Window name: " + window.GetName());
}

void PrintSelected(Window@ window)
{
  RadioButton@ radioButton = cast<RadioButton>(window);
  if (radioButton !is null)
  {
    Println("The radio button " + radioButton.GetName() + " selection is " + radioButton.IsSelected() + ".");
  }
}