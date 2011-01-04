/// Representation of the current game. It is registered as a no-handle reference. So, the only
/// way you can access it is to use the ::game global variable.
/// The game can hold some dynamic properties. It is a place where you can store data such as total score or time.
class Game
{
public:

	/// Clears the dynamic property list assigned to the game itself.
	void ClearDynamicProperties();

	/// Returns whether the game has the given property.
	bool HasDynamicProperty(const string& property) const;

	/// Deletes the given dynamic property from the game.
	bool DeleteDynamicProperty(const string& name);

	/// Loads the game from the given filename.
	bool LoadFromFile(const string& fileName);

	/// Saves the game to the given filename.
	bool SaveToFile(const string& fileName);

	/// Pauses the game action until it is resumed again.
	void PauseAction();

	/// Resumes the game action if it's paused.
	void ResumeAction();

	/// Quits the game.
	void Quit();

	/// Returns the current game time in milliseconds.
	uint64 GetTime();

	/// Returns true if the game is running in fullscreen.
	bool GetFullscreen();

	/// Toggles the fullscreen mode.
	void SetFullscreen(bool fullscreen);

#define G_METHODS(TYPE) \
	/** @name Property Manipulation */ \
	/** @{ */ \
	/** Returns the TYPE value of the given game property. */ \
	TYPE Get_##TYPE(const string& propertyName); \
	/** Sets the TYPE value of the given game property. */ \
	void Set_##TYPE(const string& propertyName, TYPE value); \
	/** Returns the array_##TYPE value of the given game property. */ \
	array_##TYPE Get_array_##TYPE(const string& propertyName) const; \
	/** Returns the const array_##TYPE value of the given game property. */ \
	const array_##TYPE Get_const_array_##TYPE(const string& propertyName) const; \
	/** Sets the array_##TYPE value of the given game property. */ \
	void Set_array_##TYPE(const string& propertyName, array_##TYPE value); \
	/** @} */


	G_METHODS(bool);
	G_METHODS(int8);
	G_METHODS(int16);
	G_METHODS(int32);
	G_METHODS(int64);
	G_METHODS(uint8);
	G_METHODS(uint16);
	G_METHODS(uint32);
	G_METHODS(uint64);
	G_METHODS(float32);
	G_METHODS(Vector2);
	G_METHODS(Point);
	G_METHODS(string);
	G_METHODS(StringKey);
	G_METHODS(EntityHandle);
	G_METHODS(Color);
	G_METHODS(eKeyCode);
};