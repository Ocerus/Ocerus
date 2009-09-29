#include "Common.h"
#include "PropertyHolder.h"


void PropertyHolder::ReportUndefined( void )
{
	gLogMgr.LogMessage("PropertyHolder:Undefined property", LOG_ERROR);
}

bool PropertyHolder::IsValid( void ) const
{
	return (mOwner!=0 && mProperty!=0);
}