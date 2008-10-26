#ifndef _COMPONENTDESCRIPTION_H_
#define _COMPONENTDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
#include "ComponentEnums.h"

namespace EntitySystem
{
	class ComponentDescriptionItem
	{
	public:
		ComponentDescriptionItem(void);
		ComponentDescriptionItem(const int8 data);
		ComponentDescriptionItem(const int16 data);
		ComponentDescriptionItem(const int32 data);
		ComponentDescriptionItem(const string& data);
		~ComponentDescriptionItem(void);

		inline bool IsDefined(void) const { return mIsDefined; }

		int8 GetInt8(void) const;
		int16 GetInt16(void) const;
		int32 GetInt32(void) const;
		string& GetString(void) const;
	private:
		bool mIsDefined;
		void* mData;	
		enum eType { TYPE_INT8, TYPE_INT16, TYPE_INT32, TYPE_STRING };
		eType mType; // for internal security checks
	};

	class ComponentDescription
	{
	public:
		ComponentDescription(eComponentType type);
		~ComponentDescription(void);

		inline eComponentType GetType() const { return mType; }
		void AddItem(ComponentDescriptionItem* item);
		ComponentDescriptionItem* GetNextItem(void);

	private:
		typedef std::vector<ComponentDescriptionItem*> DescriptionItems;

		DescriptionItems mItems;
		eComponentType mType;
		uint32 mIndex;
	};
}

#endif