#include "Common.h"
#include "AbstractProperty.h"


void AbstractProperty::ReportConvertProblem( ePropertyType wrongType ) const
{
	ocError << "Can't convert property '" << GetName() << "' from '" << PropertyTypes::GetStringName(GetType()) << "' to '" << PropertyTypes::GetStringName(wrongType) << "'";
}

void AbstractProperty::ReportReadonlyProblem( void ) const
{
	ocWarning << "Property '" << GetName() << "' is read-only";
}

void AbstractProperty::ReportWriteonlyProblem( void ) const
{
	ocWarning << "Property '" << GetName() << "' is write-only";
}

string AbstractProperty::GetValueString(const Reflection::RTTIBaseClass* owner) const
{
	switch (GetType())
	{
		case PT_BOOL:
			return StringConverter::ToString(GetValue<bool>(owner));
		case PT_BOOL_ARRAY:
			return StringConverter::ToString(GetValue<Array<bool>*>(owner));
		case PT_FLOAT32:
			return StringConverter::ToString(GetValue<float32>(owner));
		case PT_FLOAT32_ARRAY:
			return StringConverter::ToString(GetValue<Array<float32>*>(owner));
		case PT_INT16:
			return StringConverter::ToString(GetValue<int16>(owner));
		case PT_INT16_ARRAY:
			return StringConverter::ToString(GetValue<Array<int16>*>(owner));
		case PT_INT32:
			return StringConverter::ToString(GetValue<int32>(owner));
		case PT_INT32_ARRAY:
			return StringConverter::ToString(GetValue<Array<int32>*>(owner));
		case PT_INT64:
			return StringConverter::ToString(GetValue<int64>(owner));
		case PT_INT64_ARRAY:
			return StringConverter::ToString(GetValue<Array<int64>*>(owner));
		case PT_INT8:
			return StringConverter::ToString(GetValue<int8>(owner));
		case PT_INT8_ARRAY:
			return StringConverter::ToString(GetValue<Array<int8>*>(owner));
		case PT_UINT16:
			return StringConverter::ToString(GetValue<uint16>(owner));
		case PT_UINT16_ARRAY:
			return StringConverter::ToString(GetValue<Array<uint16>*>(owner));
		case PT_UINT32:
			return StringConverter::ToString(GetValue<uint32>(owner));
		case PT_UINT32_ARRAY:
			return StringConverter::ToString(GetValue<Array<uint32>*>(owner));
		case PT_UINT64:
			return StringConverter::ToString(GetValue<uint64>(owner));
		case PT_UINT64_ARRAY:
			return StringConverter::ToString(GetValue<Array<uint64>*>(owner));
		case PT_UINT8:
			return StringConverter::ToString(GetValue<uint8>(owner));
		case PT_UINT8_ARRAY:
			return StringConverter::ToString(GetValue<Array<uint8>*>(owner));
		case PT_STRING:
			return GetValue<string>(owner);
		case PT_STRING_ARRAY:
			return StringConverter::ToString(GetValue<Array<string>*>(owner));
		case PT_STRING_KEY:
			return StringConverter::ToString(GetValue<StringKey>(owner));
		case PT_STRING_KEY_ARRAY:
			return StringConverter::ToString(GetValue<Array<StringKey>*>(owner));
		case PT_COLOR:
			return StringConverter::ToString(GetValue<GfxSystem::Color>(owner));
		case PT_COLOR_ARRAY:
			return StringConverter::ToString(GetValue<Array<GfxSystem::Color>*>(owner));
		case PT_VECTOR2:
			return StringConverter::ToString(GetValue<Vector2>(owner));
		case PT_VECTOR2_ARRAY:
			return StringConverter::ToString(GetValue<Array<Vector2>*>(owner));
		case PT_ENTITYHANDLE:
			if (EntitySystem::EntityMgr::SingletonExists())
			{
				return EntitySystem::EntityMgr::GetSingleton().GetEntityName(GetValue<EntitySystem::EntityHandle>(owner));
			}
			else
			{
				return StringConverter::ToString(GetValue<EntitySystem::EntityHandle>(owner).GetID());
			}
		default:
			ocError << "Converting property of type '" << PropertyTypes::GetStringName(GetType()) << "' to string is not implemented";
	}
	return "";
}

void Reflection::AbstractProperty::SetValueFromString( RTTIBaseClass* owner, const string& str )
{
    switch (GetType())
	{
	case PT_BOOL:
		SetValue(owner, StringConverter::FromString<bool>(str));
		break;
	case PT_FLOAT32:
		SetValue(owner, StringConverter::FromString<float32>(str));
		break;
	case PT_INT16:
		SetValue(owner, StringConverter::FromString<int16>(str));
		break;
	case PT_INT32:
		SetValue(owner, StringConverter::FromString<int32>(str));
		break;
	case PT_INT64:
		SetValue(owner, StringConverter::FromString<int64>(str));
		break;
	case PT_INT8:
		SetValue(owner, StringConverter::FromString<int8>(str));
		break;
	case PT_UINT16:
		SetValue(owner, StringConverter::FromString<uint16>(str));
		break;
	case PT_UINT32:
		SetValue(owner, StringConverter::FromString<uint32>(str));
		break;
	case PT_UINT64:
		SetValue(owner, StringConverter::FromString<uint64>(str));
		break;
	case PT_UINT8:
		SetValue(owner, StringConverter::FromString<uint8>(str));
		break;
	case PT_STRING:
		SetValue(owner, str);
		break;
	case PT_STRING_KEY:
		SetValue<StringKey>(owner, StringConverter::FromString<string>(str));
		break;
	case PT_COLOR:
		SetValue(owner, StringConverter::FromString<GfxSystem::Color>(str));
		break;
	case PT_VECTOR2:
		SetValue(owner, StringConverter::FromString<Vector2>(str));
		break;
	case PT_ENTITYHANDLE:
		{
			EntitySystem::EntityHandle e = gEntityMgr.FindFirstEntity(str);
			if (!e.IsValid()) ocError << "Can't parse property of EntityHandle '" << PropertyTypes::GetStringName(GetType()) << "' from string '" << str << "'; it could not been found";
			SetValue(owner, e);
			break;
		}
	default:
		ocError << "Parsing property of type '" << PropertyTypes::GetStringName(GetType()) << "' from string is not implemented";
    }
}
