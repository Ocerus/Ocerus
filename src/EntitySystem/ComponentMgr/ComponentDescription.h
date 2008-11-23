#ifndef _COMPONENTDESCRIPTION_H_
#define _COMPONENTDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
#include "ComponentEnums.h"
#include "ComponentDescriptionItem.h"
#include "../EntityMgr/EntityHandle.h"

namespace EntitySystem
{

	/** This class holds info needed to create one specific component along with its type.
	*/
	class ComponentDescription
	{
	public:
		/// Component types are located in ComponentEnums.h
		ComponentDescription();
		~ComponentDescription(void);

		/// Call this before each description filling.
		void Init(eComponentType type);

		/// Getters.
		inline eComponentType GetType(void) const { return mType; }

		/// Adds a data item into this description.
		template<class DataType>
		void AddItem(DataType data)
		{
			mItems->push_back(DYN_NEW ComponentDescriptionItem(data));
		}

		/// Returns next item in the list. For internal use by the component.
		ComponentDescriptionItem* GetNextItem(void);

	private:
		friend class EntityDescription;

		typedef std::vector<ComponentDescriptionItem*> DescriptionItems;

		DescriptionItems* mItems;
		eComponentType mType;
		uint32 mIndex;

		/// Copy constructor to be used only by EntityDescription.
		ComponentDescription(eComponentType type, DescriptionItems* items);

		/// Sets this description to an invalid state.
		void Invalidate(void);

		/// Returns pointer to a vector containing internal items.
		DescriptionItems* GetItems(void) const { return mItems; }

		/// Deletes everything.
		void Clear(void);
	};
}

#endif