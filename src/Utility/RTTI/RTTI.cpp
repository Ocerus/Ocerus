#include "Common.h"
#include "RTTI.h"

RTTI::RTTI(	uint8 dwStub, ClassID CLID, const char* szClassName, RTTI* pBaseClassRTTI, 
				ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc ) : 
	mCLID			( CLID				),
	mBaseRTTI		( pBaseClassRTTI	),
	mClassFactory( pFactory			),
	mComponentDependencies(0)
{
	assert( CLID != 0 );
	assert( strlen(szClassName) <= CLASSNAME_LENGTH && "Class name too long" );
	#pragma warning(disable: 4996)
	strcpy( mClassName, szClassName );

	if ( pReflectionFunc )
		pReflectionFunc();
}

void RTTI::EnumProperties( AbstractPropertyList& out, const uint8 flagMask )
{
	if ( mBaseRTTI )
		mBaseRTTI->EnumProperties( out, flagMask );
	for ( std::list<AbstractProperty*>::iterator it = mProperties.begin(); it != mProperties.end(); ++it )
		if (((*it)->GetAccessFlags()&flagMask) == flagMask)
			out.push_back( *it );
}

void RTTI::EnumProperties( RTTIBaseClass* owner, PropertyList& out, const uint8 flagMask )
{
	if ( mBaseRTTI )
		mBaseRTTI->EnumProperties( owner, out, flagMask );
	for ( std::list<AbstractProperty*>::iterator it = mProperties.begin(); it != mProperties.end(); ++it )
		if (((*it)->GetAccessFlags()&flagMask) == flagMask)
		{
			const char* name = (*it)->GetName();
			if (out.find(name) != out.end())
				gLogMgr.LogMessage("Duplicate property name '", name, "' -> overwriting", LOG_ERROR);
			out[name] = PropertyHolder(owner, *it);
		}
}

void RTTI::EnumComponentDependencies( ComponentDependencyList& out )
{
	if ( mBaseRTTI )
		mBaseRTTI->EnumComponentDependencies(out);
	for (ComponentDependencyList::const_iterator it=mComponentDependencies.begin(); it!=mComponentDependencies.end(); ++it )
		out.push_back(*it);
}