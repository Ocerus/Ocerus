#ifndef _COMPONENTDESCRIPTIONITEM_H_
#define _COMPONENTDESCRIPTIONITEM_H_

namespace EntitySystem
{

	/** This class represents a specific data element.
	*/
	class ComponentDescriptionItem
	{
	public:
		/// Constructor taking data to be held by this item. Type of the item is determined automatically.
		template<class T>
		ComponentDescriptionItem(T data): mData(0)
		{
			mData = DYN_NEW T(data);
			mType = DetermineType<T>();
			assert(mType != TYPE_UNKNOWN && "Unknown item type");
		}

		/// Destroy everything.
		~ComponentDescriptionItem(void)
		{
			if (mData)
				DYN_DELETE mData;
		}

		/// Getters.
		template<class T>
		T GetData(void) const
		{
			assert(mType == DetermineType<T>() && "Wrong item type");
			return *static_cast<T*>(mData);
		}

	private:
		/// Pointer to the actual data.
		void* mData;	

		/// For internal security checks.
		//@{
		enum eType 
		{ 
			TYPE_BOOL,
			TYPE_INT8,
			TYPE_INT16, 
			TYPE_INT32, 
			TYPE_INT64, 
			TYPE_UINT8, 
			TYPE_UINT16,
			TYPE_UINT32,
			TYPE_UINT64, 
			TYPE_FLOAT32, 
			TYPE_VECTOR2, 
			TYPE_VECTOR2_ARRAY, 
			TYPE_ENTITYHANDLE,
			TYPE_STRING,

			TYPE_UNKNOWN
		};
		eType mType; 
		//@}

		/// Automatically determines type of this item.
		template<class T> inline eType DetermineType(void) const { return TYPE_UNKNOWN; }
	};
	
template<> inline ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<bool>() const { return ComponentDescriptionItem::TYPE_BOOL; }
template<> inline ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<int8>() const { return ComponentDescriptionItem::TYPE_INT8; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<int16>() const { return ComponentDescriptionItem::TYPE_INT16; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<int32>() const { return ComponentDescriptionItem::TYPE_INT32; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<int64>() const { return ComponentDescriptionItem::TYPE_INT64; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<uint8>() const { return ComponentDescriptionItem::TYPE_UINT8; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<uint16>() const { return ComponentDescriptionItem::TYPE_UINT16; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<uint32>() const { return ComponentDescriptionItem::TYPE_UINT32; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<uint64>() const { return ComponentDescriptionItem::TYPE_UINT64; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<float32>() const { return ComponentDescriptionItem::TYPE_FLOAT32; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<Vector2>() const { return ComponentDescriptionItem::TYPE_VECTOR2; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<Vector2*>() const { return ComponentDescriptionItem::TYPE_VECTOR2_ARRAY; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<EntityHandle>() const { return ComponentDescriptionItem::TYPE_ENTITYHANDLE; }
template<> inline  ComponentDescriptionItem::eType
ComponentDescriptionItem::DetermineType<char*>() const { return ComponentDescriptionItem::TYPE_STRING; }

}

#endif