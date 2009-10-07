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

		/// Creates a picker for selecting an entity under the current mouse cursor.
		EntityPicker(const int32 screenCursorX, const int32 screenCursorY);

		/// Tries to pick this entity if it is in specified area and has a specified type.
		void Update(const EntityHandle entity, const Vector2& circleCenter, const float32 circleRadius);

		/// Tries to pick this entity if it is in specified area and has a specified type.
		void Update(const EntityHandle entity, const float32 rectX1, const float32 rectY1, const float32 rectX2, const float32 rectY2);

		/// Returns result of the search.
		EntityHandle GetResult(void) const;

		/// Sets priorities for all entity types in the system.
		static void SetupPriorities(void);

		/// Cleans the priority data of all entity types in the system.
		static void CleanPriorities(void);

	private:

		/// Priorities of entity types
		static uint8* msPriorities;
		
		/// Picking data.
		Vector2 mCursorWorldPosition;

		/// Found entities.
		EntityHandle mResult;
		eEntityType mResultType;

		void PickIfHigherPriority(const EntityHandle entity);

		/// Disabled.
		EntityPicker(const EntityPicker& rhs) {}
		EntityPicker& operator=(const EntityPicker& rhs) {}
	};
}

#endif // EntityPicker_h__