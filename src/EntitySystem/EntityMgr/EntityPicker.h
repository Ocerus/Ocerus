#ifndef EntityPicker_h__
#define EntityPicker_h__

#include "Settings.h"

namespace EntitySystem
{
	/// @name This class allows the game to pick (choose) and entity based of provided input data (ie. a mouse cursor position).
	class EntityPicker
	{
	public:
		//TODO pridat filter na typy entit, ktery to muze picknout
		//TODO pridat moznost definovat celou oblast misto pouhyho kursoru
		EntityPicker(const Vector2& worldCursorPos);
		EntityPicker(const int32 screenCursorX, const int32 screenCursorY);

		/// @name Tries to pick this entity if it is in specified area and has a specified type.
		//@{
		void Update(const EntityHandle entity, const Vector2& circleCenter, const float32 circleRadius);
		void Update(const EntityHandle entity, const float32 rectX1, const float32 rectY1, const float32 rectX2, const float32 rectY2);
		//@}

		//TODO pridat moznost vzit vic vysledku (pri definovani vetsi oblasti pouze)
		/// @name Returns result of the search.
		EntityHandle GetResult(void) const;

		/// @name Sets up priorities for all entity types.
		static void SetupPriorities(void);
		static void CleanPriorities(void);

	private:

		/// @name Priorities of entity types
		static uint8* msPriorities;
		
		/// @name Picking data.
		Vector2 mCursorWorldPosition;

		/// @name Found entities.
		//@{
		EntityHandle mResult;
		eEntityType mResultType;
		//@}

		void PickIfHigherPriority(const EntityHandle entity);

		/// @name Disabled.
		//@{
		EntityPicker(const EntityPicker& rhs) {}
		EntityPicker& operator=(const EntityPicker& rhs) {}
		//@}
	};
}

#endif // EntityPicker_h__