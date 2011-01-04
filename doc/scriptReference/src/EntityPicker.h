/// Allows to pick an entity or multiple entities based on the provided data (such as a position).
/// This class is registered as a value type.
class EntityPicker
{
public:

	/// Creates a picker for selecting and entity under the current mouse cursor.
	/// The cursor position is given in world coordinates. The minimum and maximum layers limit
	/// the picked entities to be only from specific layer range.
	EntityPicker(const Vector2& cursorWorldPosition, int32 minimumLayer, int32 maximumLayer);

	/// Runs the picking query and returns the result directly.
	EntityHandle PickSingleEntity();

	/// Runs the picking query and fills the result into the given array. The query is defined by a rectangle between
	/// the last cursor position and the given cursor position. The rectangle is rotated by the given angle.
	/// The out array is resized to the number of returned entities.
	void PickMultipleEntities(EntityHandle[]& out, const Vector2& worldPosition, float32 rectangleAngle);
};