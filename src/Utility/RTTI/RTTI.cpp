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
	m_pObjectFactory( pFactory			)
{
	assert( CLID != 0 );
	assert( strlen(szClassName) <= CLASSNAME_LENGTH );
	#pragma warning(disable: 4996)
	strcpy( m_szClassName, szClassName );

	if ( pReflectionFunc )
		pReflectionFunc();
}

void CRTTI::EnumProperties( std::vector<CAbstractProperty*>& o_Result )
{
	if ( m_pBaseRTTI )
		m_pBaseRTTI->EnumProperties( o_Result );
	for ( std::list<CAbstractProperty*>::iterator it = m_Properties.begin(); it != m_Properties.end(); ++it )
		o_Result.push_back( *it );
}