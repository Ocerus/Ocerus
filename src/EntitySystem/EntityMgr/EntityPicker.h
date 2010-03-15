/// @file
/// System for enumerating entities based on some criteria (e.g. their spatial configuration).

#ifndef EntityPicker_h__
#define EntityPicker_h__

#include "Base.h"

namespace EntitySystem
{
	/// This class allows the game to pick an entity based on provided input data (ie. a mouse cursor position).
	class EntityPicker
	{
	public:

		/// Creates a picker for selecting an entity under the current mouse cursor.
		EntityPicker(const Vector2& worldCursorPos);

		/// Runs the picking query. The result is returned directly.
		EntityHandle PickSingleEntity(void);

		/// Runs the picking query. The result is filled into the given vector.
		/// The query is defined by a rectangle between the last cursor position and the given cursor position.
		/// The rectangle is rotated by the given angle.
		/// Returns the number of picked entities.
		uint32 PickMultipleEntities(const Vector2& worldCursorPos, const float32 rotation, vector<EntityHandle>& out);

	private:

		/// Picking data.
		Vector2 mCursorWorldPosition;

		/// Found entities.
		EntityHandle mResult;

		/// Disabled.
		EntityPicker(const EntityPicker& rhs);
		EntityPicker& operator=(const EntityPicker& rhs);
	};
}

#endif // EntityPicker_h__