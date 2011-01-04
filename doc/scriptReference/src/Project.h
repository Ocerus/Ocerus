/// Representation of the current game project. It is registered as a no-handle reference. So, the only
/// way you can access it is to use the ::gProject global variable.
class Project
{
public:

	/// Opens a scene with the given filename.
	bool OpenScene(const string& name);

	/// Opens the scene at the given index in the scene list.
	bool OpenSceneAtIndex(int32 index);

	/// Returns the number of scenes in the scene list.
	uint32 GetSceneCount() const;

	/// Returns the index of the scene with the given name. Returns -1 if it doesn't exist.
	int32 GetSceneIndex(const string& name) const;

	/// Returns the name of the opened scene. Returns empty string if no scene is opened.
	string GetOpenedSceneName() const;

	/// Returns the name of the scene at the given index in the scene list.
	string GetSceneName(int32 index) const;
};