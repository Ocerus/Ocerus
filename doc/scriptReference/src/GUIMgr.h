/// Management of the graphical user interface of the game. It is registered as a no-handle reference. So, the only
/// way you can access it is to use the ::gGUIMgr global variable.
class GUIMgr
{
public:

	/// Loads the project scheme file.
	void LoadScheme(const string& filename);

	/// Loads the project imageset file.
	void LoadImageset(const string& filename);

	/// Loads the project layout file and returns it as a root window. The widget names in the layout file
	/// are prefixed with the given prefix.
	Window* LoadLayout(const CEGUIString& layout, const CEGUIString& widgetPrefix);
};