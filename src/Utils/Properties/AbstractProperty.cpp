#include "Common.h"
#include "AbstractProperty.h"


void AbstractProperty::ReportConvertProblem( ePropertyType wrongType )
{
	gLogMgr.LogMessage("Can't convert property '", GetName(), "' from '", GetType(), "' to '", wrongType, "'", LOG_ERROR);
}

void AbstractProperty::ReportReadonlyProblem( void )
{
	gLogMgr.LogMessage("Property '", GetName(), "' is read-only", LOG_ERROR);
}

void AbstractProperty::ReportWriteonlyProblem( void )
{
	gLogMgr.LogMessage("Property '", GetName(), "' is write-only", LOG_ERROR);
}