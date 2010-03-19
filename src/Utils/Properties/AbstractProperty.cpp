#include "Common.h"
#include "AbstractProperty.h"
#include "../../ResourceSystem/XMLOutput.h"
#include "../XMLConverter.h"


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
	// We generate cases for all property types and arrays of property types here.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID: \
		return StringConverter::ToString(GetValue<typeClass>(owner));
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID##_ARRAY: \
		return StringConverter::ToString(GetValue<Array<typeClass>*>(owner));
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	default:
		OC_NOT_REACHED();
	}
	return "";
}

void AbstractProperty::WriteValueXML(const RTTIBaseClass* owner, ResourceSystem::XMLOutput& output) const
{
	
	switch (GetType())
	{
	// We generate cases for all property types and arrays of property types here.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID: \
		Utils::XMLConverter::WriteToXML(output, GetValue<typeClass>(owner)); break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID##_ARRAY: \
		Utils::XMLConverter::WriteToXML(output, GetValue<Array<typeClass>*>(owner)); break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	default:
		OC_NOT_REACHED();
	}
}

/// @todo: Use StringConverter here instead.
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

template<typename T>
void ReadArrayValueXML(Reflection::AbstractProperty* prop, RTTIBaseClass* owner, ResourceSystem::XMLNodeIterator& input)
{
	vector<T> vertices;
	for (ResourceSystem::XMLNodeIterator vertIt = input.IterateChildren(); vertIt != input.EndChildren(); ++vertIt)
	{
		if ((*vertIt).compare("Item") == 0) { vertices.push_back(Utils::XMLConverter::ReadFromXML<T>(vertIt)); }
		else ocError << "XML:Entity: Expected 'Item', found '" << *vertIt << "'";
	}

	Array<T> vertArray(vertices.size());
	for (uint32 i=0; i<vertices.size(); ++i)
	{
		vertArray[i] = vertices[i];
	}
	prop->SetValue<Array<T>*>(owner, &vertArray);
}

void Reflection::AbstractProperty::ReadValueXML(RTTIBaseClass* owner, ResourceSystem::XMLNodeIterator& input)
{
	int32 a = Utils::XMLConverter::ReadFromXML<int32>(input);
	if (a == 0) {}
	switch (GetType())
	{
	// We generate cases for all property types and arrays of property types here.
	#define SCRIPT_ONLY
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID: \
		SetValue<typeClass>(owner, Utils::XMLConverter::ReadFromXML<typeClass>(input)); break;
    #include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID##_ARRAY: \
		ReadArrayValueXML<typeClass>(this, owner, input); break;
		//SetValue<Array<typeClass>*>(owner, Utils::XMLConverter::ReadFromXML<Array<typeClass>*>(input)); break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE
	#undef SCRIPT_ONLY

	default:
		OC_NOT_REACHED();
	}
}
