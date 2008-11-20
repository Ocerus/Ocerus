#ifndef _COMPONENTDESCRIPTION_H_
#define _COMPONENTDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
#include "ComponentEnums.h"
#include "../EntityMgr/EntityHandle.h"

namespace EntitySystem
{
	/** This class represents a specific data element.
	*/
	class ComponentDescriptionItem
	{
	public:
		/// Constructors taking data to be held by this item.
		//@{
		ComponentDescriptionItem(const int8 data);
		ComponentDescriptionItem(const int16 data);
		ComponentDescriptionItem(const int32 data);
		ComponentDescriptionItem(const int64 data);
		ComponentDescriptionItem(const uint8 data);
		ComponentDescriptionItem(const uint16 data);
		ComponentDescriptionItem(const uint32 data);
		ComponentDescriptionItem(const uint64 data);
		ComponentDescriptionItem(const float32 data);
		ComponentDescriptionItem(const EntityHandle data);
		ComponentDescriptionItem(const string& data);
		//@}
		~ComponentDescriptionItem(void);

		/// Getters.
		//@{
		int8 GetInt8(void) const;
		int16 GetInt16(void) const;
		int32 GetInt32(void) const;
		int64 GetInt64(void) const;
		uint8 GetUint8(void) const;
		uint16 GetUint16(void) const;
		uint32 GetUint32(void) const;
		uint64 GetUint64(void) const;
		float32 GetFloat32(void) const;
		EntityHandle GetEntityHandle(void) const;
		string GetString(void) const;
		//@}
	private:
		/// Pointer to the actual data.
		void* mData;	
		/// For internal security checks.
		//@{
		enum eType { TYPE_INT8, TYPE_INT16, TYPE_INT32, TYPE_INT64, TYPE_UINT8, TYPE_UINT16, TYPE_UINT32, TYPE_UINT64, TYPE_FLOAT32, TYPE_ENTITYHANDLE, TYPE_STRING };
		eType mType; 
		//@}
	};

	/** This class holds info needed to create one specific component along with its type.
	*/
	class ComponentDescription
	{
	public:
		/// Component types are located in ComponentEnums.h
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