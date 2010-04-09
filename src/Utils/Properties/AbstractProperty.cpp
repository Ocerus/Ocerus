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
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID: \
		return StringConverter::ToString(GetValue<typeClass>(owner));
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID##_ARRAY: \
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
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID: \
		Utils::XMLConverter::WriteToXML(output, GetValue<typeClass>(owner)); break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID##_ARRAY: \
		Utils::XMLConverter::WriteToXML(output, GetValue<Array<typeClass>*>(owner)); break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	default:
		OC_NOT_REACHED();
	}
}

void Reflection::AbstractProperty::SetValueFromString( RTTIBaseClass* owner, const string& str )
{
	switch (GetType())
	{
	// We generate cases for all property types and arrays of property types here.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
	case typeID: \
		SetValue(owner, StringConverter::FromString<typeClass>(str)); \
		break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	default:
		ocError << "Parsing property of type '" << PropertyTypes::GetStringName(GetType()) << "' from string is not implemented.";
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
	switch (GetType())
	{
	// We generate cases for all property types and arrays of property types here.
	#define SCRIPT_ONLY
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
    case typeID: \
		SetValue<typeClass>(owner, Utils::XMLConverter::ReadFromXML<typeClass>(input)); \
		break;
    #include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
	case typeID##_ARRAY: \
		ReadArrayValueXML<typeClass>(this, owner, input); \
		break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE
	#undef SCRIPT_ONLY

	case PT_RESOURCE:
		{
			string inputText = Utils::XMLConverter::ReadFromXML<string>(input);
			SetValue<ResourceSystem::ResourcePtr>(owner, gResourceMgr.GetResource(inputText.c_str()));
			break;
		}

	default:
		ocError << "Parsing property of type '" << PropertyTypes::GetStringName(GetType()) << "' from XML is not implemented.";
	}
}
