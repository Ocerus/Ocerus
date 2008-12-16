#include "Common.h"
#include "AbstractProperty.h"


void CAbstractProperty::ReportConvertProblem( ePropertyType wrongType )
{
	gLogMgr.LogMessage("Can't convert property '", GetName(), "' from '", GetType(), "' to '", wrongType, "'", LOG_ERROR);
}

void CAbstractProperty::ReportReadonlyProblem( void )
{
	gLogMgr.LogMessage("Property '", GetName(), "' is read-only", LOG_ERROR);
}

void CAbstractProperty::ReportWriteonlyProblem( void )
{
	gLogMgr.LogMessage("Property '", GetName(), "' is write-only", LOG_ERROR);
}