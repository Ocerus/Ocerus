#include "Common.h"
#include "PropertyHolder.h"


void PropertyHolder::ReportUndefined( void )
{
	gLogMgr.LogMessage("Undefined property", LOG_ERROR);
}