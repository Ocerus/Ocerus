/// Base class for all GUI elements. It is registered as a basic reference
/// and its handle can be cast to all of its descendants.
/// See CEGUI documentation for more info on the presented methods.
class Window
{
public:

	/// Returns the name of this window.
	const CEGUIString& GetName() const;

	/// Returns the type name of this window.
	const CEGUIString& GetType() const;

	/// Returns true if the window is currently disabled (does not inherit the state from ancestors).
	bool IsDisabled() const;

	/// Returns true if the window is currently disabled. You can specify if you want to inherit
	/// the state from its ancestors).
	bool IsDisabled(bool inherit) const;

	/// Returns true if the window is visible (does not inherit the state from ancestors).
	bool IsVisible() const;

	/// Returns true if the window is visible. You can specify if you want to inherit the state
	/// from its ancestors.
	bool IsVisible(bool inherit) const;

	/// Returns true if this is the active window (receives user input).
	bool IsActive() const;

	/// Returns the text the window currently displays.
	const CEGUIString& GetText() const;

	/// Sets the text for the window.
	void SetText(const CEGUIString& text);

	/// Returns true if the window inherits alhpa from its parent.
	bool InheritsAlpha() const;
	
	/// Tells the window to or not to inherit the alpha value from its parent.
	void SetInheritsAlpha(bool inherit);

	/// Returns the alpha value of this window.
	float32 GetAlpha() const;

	/// Sets the alpha value.
	void SetAlpha(float32 alpha);

	/// Returns the effective alpha value. It takes into account alpha of the parents as well.
	float32 GetEfectiveAlpha() const;

	/// Returns the parent window.
	Window@ GetParent() const;

	/// Returns the tooltip text.
	const CEGUIString& GetTooltipText() const;

	/// Sets new tooltip text.
	void SetTooltipText(const CEGUIString& tooltipText);

	/// Returns true if the window inherits the tooltip text from its parent.
	bool InheritsTooltipText() const;

	/// Tells the window to or not to inherit the tooltip from its parent.
	void SetInheritsTooltipText(bool inherit);

	/// Sets the window enabled or disabled.
	void SetEnabled(bool enabled);

	/// Sets the window visible or invisible.
	void SetVisible(bool visible);

	/// Activates the window giving it focus and bringing it to the front.
	void Activate();

	/// Deactivates the window.
	void Deactivate();

	/// Returns the value of a property of the window. The property is specified by a string name. 
	/// The available properties depend on the type of the window. To get the full list of properties you can use
	/// <a href="http://www.cegui.org.uk/wiki/index.php/PropertyFinder">this tools</a> or see the list
	/// <a href="http://www.cegui.org.uk/wiki/index.php/SetProperty">here</a>.
	CEGUIString GetProperty(const CEGUIString& propertyName) const;

	/// Sets a property of the window. The property is specified by a string name. The available properties depend
	/// on the type of the window. To get the full list of properties you can use
	/// <a href="http://www.cegui.org.uk/wiki/index.php/PropertyFinder">this tools</a> or see the list
	/// <a href="http://www.cegui.org.uk/wiki/index.php/SetProperty">here</a>.
	void SetProperty(const CEGUIString& propertyName, const CEGUIString& propertyValue);

};