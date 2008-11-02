#ifndef _COMPONENTDESCRIPTION_H_
#define _COMPONENTDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
#include "ComponentEnums.h"

namespace EntitySystem
{
	/** This class represents a specific data element.
	*/
	class ComponentDescriptionItem
	{
	public:
		ComponentDescriptionItem(const int8 data);
		ComponentDescriptionItem(const int16 data);
		ComponentDescriptionItem(const int32 data);
		ComponentDescriptionItem(const string& data);
		~ComponentDescriptionItem(void);

		/// Getters.
		//@{
		int8 GetInt8(void) const;
		int16 GetInt16(void) const;
		int32 GetInt32(void) const;
		string GetString(void) const;
		//@}
	private:
		/// Pointer to the actual data.
		void* mData;	
		/// For internal security checks.
		//@{
		enum eType { TYPE_INT8, TYPE_INT16, TYPE_INT32, TYPE_STRING };
		eType mType; 
		//@}
	};

	/** This class holds info needed to create one specific component along with its type.
	*/
	class ComponentDescription
	{
	public:
		ComponentDescription(eComponentType type);
		~ComponentDescription(void);

		/// Getters.
		inline eComponentType GetType() const { return mType; }

		/// Adds a data item into this description.
		void AddItem(ComponentDescriptionItem* item);

		/// Returns next item in the list. For internal use by the component.
		ComponentDescriptionItem* GetNextItem(void);

	private:
		typedef std::vector<ComponentDescriptionItem*> DescriptionItems;

		DescriptionItems mItems;
		eComponentType mType;
		uint32 mIndex;
	};
}

#endif