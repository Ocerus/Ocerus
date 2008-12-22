//
// CRTTI.cpp
//

#include "Common.h"
#include "RTTI.h"

//-------------------------------------------------------------------------------------------------
CRTTI::CRTTI(	uint8 dwStub, ClassID CLID, const char* szClassName, CRTTI* pBaseClassRTTI, 
				ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc ) : 
	m_CLID			( CLID				),
	m_pBaseRTTI		( pBaseClassRTTI	),
	m_pObjectFactory( pFactory			),
	m_ComponentDependencies(0)
{
	assert( CLID != 0 );
	assert( strlen(szClassName) <= CLASSNAME_LENGTH && "Class name too long" );
	#pragma warning(disable: 4996)
	strcpy( m_szClassName, szClassName );

	if ( pReflectionFunc )
		pReflectionFunc();
}

void CRTTI::EnumProperties( AbstractPropertyList& o_Result, const uint8 flagMask )
{
	if ( m_pBaseRTTI )
		m_pBaseRTTI->EnumProperties( o_Result, flagMask );
	for ( std::list<CAbstractProperty*>::iterator it = m_Properties.begin(); it != m_Properties.end(); ++it )
		if (((*it)->GetAccessFlags()&flagMask) == flagMask)
			o_Result.push_back( *it );
}

void CRTTI::EnumProperties( RTTIBaseClass* owner, PropertyList& o_Result, const uint8 flagMask )
{
	if ( m_pBaseRTTI )
		m_pBaseRTTI->EnumProperties( owner, o_Result, flagMask );
	for ( std::list<CAbstractProperty*>::iterator it = m_Properties.begin(); it != m_Properties.end(); ++it )
		if (((*it)->GetAccessFlags()&flagMask) == flagMask)
		{
			const char* name = (*it)->GetName();
			if (o_Result.find(name) != o_Result.end())
				gLogMgr.LogMessage("Duplicate property name '", name, "' -> overwriting", LOG_ERROR);
			o_Result[name] = PropertyHolder(owner, *it);
		}
}

void CRTTI::EnumComponentDependencies( ComponentDependencyList& out )
{
	if ( m_pBaseRTTI )
		m_pBaseRTTI->EnumComponentDependencies(out);
	for (ComponentDependencyList::const_iterator it=m_ComponentDependencies.begin(); it!=m_ComponentDependencies.end(); ++it )
		out.push_back(*it);
}