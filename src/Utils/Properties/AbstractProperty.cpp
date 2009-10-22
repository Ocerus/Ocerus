#include "Common.h"
#include "AbstractProperty.h"


void AbstractProperty::ReportConvertProblem( ePropertyType wrongType )
{
	gLogMgr.LogMessage("Can't convert property '", GetName(), "' from '", GetType(), "' to '", wrongType, "'", LOG_ERROR);
	OC_ASSERT(false);
}

void AbstractProperty::ReportReadonlyProblem( void )
{
	gLogMgr.LogMessage("Property '", GetName(), "' is read-only", LOG_ERROR);
}

void AbstractProperty::ReportWriteonlyProblem( void )
{
	gLogMgr.LogMessage("Property '", GetName(), "' is write-only", LOG_ERROR);
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
		SetValue(owner, StringConverter::FromString<char*>(str));
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
	case PT_VECTOR2_ARRAY:
	case PT_ENTITYHANDLE:
	case PT_ENTITYHANDLE_ARRAY:
		gLogMgr.LogMessage("Can't parse property of type '", GetType(), "' from string", LOG_ERROR);
		break;
	default:
		gLogMgr.LogMessage("Parsing property of type '", GetType(), "' from string is not implemented in the file ", __FILE__, LOG_ERROR);
    }
}
