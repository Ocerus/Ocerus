#ifndef __RTTI_H__
#define __RTTI_H__

#include <list>
#include <vector>

#include "../Properties/AbstractProperty.h"
#include "../Properties/PropertyHolder.h"
#include "../Settings.h"
#include "../../EntitySystem/ComponentMgr/ComponentEnums.h"

/// @name Forward declarations.
//@{
class RTTIBaseClass;
class RTTI;
//@}

/// @name Type of the identifier of a class.
typedef uint32 ClassID;
/// @name Factory function used for creating an instance of an owner class.
typedef RTTIBaseClass* (*ClassFactoryFunc)();
/// @name This functions is called by client classes to register properties into RTTI.
typedef bool (*RegisterReflectionFunc)();
/// @name A list of component types determining dependency of a component on other components.
typedef std::vector<EntitySystem::eComponentType> ComponentDependencyList;

/// @name Maximum length of a name of a class.
const uint32 CLASSNAME_LENGTH = 48;

/// @name RTTI description structure.
class RTTI
{
public:

	/** RTTI constructor.
		The first paramter is a stub. I am not sure why this stub is necessary - removing ths stub will
		confuse the .NET compiler and produce compile errors with subsequent parameters. If anybody knows 
		why this is so, feel free to e-mail me at dfilion@hotmail.com

		The RTTI structure constructor takes in the following parameters:
		dwStub			Just a stub
		CLID			A unique class ID
		szClassName		The name of the class type this RTTI structure represents
		pBaseClassRTTI	The parent RTTI structure for this RTTI structure
		pFactory		A factory function callback for creating an instance of the bound class type
		pReflectionFunc	Callback called by the system to register the reflective properties
	*/
	RTTI(uint8 dwStub, ClassID CLID, const char* szClassName, RTTI* pBaseClassRTTI, 
			ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc);

	/** @name Fills a data structure with all properties/component dependencies of the represented class type, 
		including all ancestor types.
	*/
	//@{
	void EnumProperties( AbstractPropertyList& out, const uint8 flagMask = 0xff );
	void EnumProperties( RTTIBaseClass* owner, PropertyList& out, const uint8 flagMask = 0xff );
	void EnumComponentDependencies(ComponentDependencyList& out);
	//@}

	/// @name Returns true if the RTTI structure is of the type specified by CLID.
	inline bool	IsTypeOf(ClassID CLID) const { return mCLID == CLID; }

	// Returns true if the RTTI structure is derived from the type specified by CLID.
	inline bool	IsDerivedFrom(ClassID CLID) const
	{
		if( mCLID == CLID )
			return true;
		else if( mBaseRTTI )
			return mBaseRTTI->IsDerivedFrom( CLID );
		return false;
	}

	/// @name Gets base class' RTTI structure.
	inline RTTI* GetAncestorRTTI(void) const { mBaseRTTI; }

	/// @name Gets the class ID.
	inline ClassID GetCLID(void) const { return mCLID; }

	/// @name Gets the class name.
	inline const char* GetClassName(void) const { return mClassName; }

	/// @name Gets the class factory function.
	inline ClassFactoryFunc	GetClassFactory(void) const { return mClassFactory; }

	/** @name Provides access to the properties/dependencies bound to this run-time type. Does not include ancestor 
		class properties. Use EnumProperties to include ancestor properties.
	*/
	//@{
	inline	std::list<AbstractProperty*>::iterator	GetFirstProperty(void) { return mProperties.begin(); }
	inline	std::list<AbstractProperty*>::iterator	GetLastProperty(void) { return mProperties.end(); }
	inline	std::list<AbstractProperty*>*			GetProperties(void) { return &mProperties; }
	inline  ComponentDependencyList* GetComponentDependencies(void) { return &mComponentDependencies; }
	//@}

private:

	ClassID	mCLID;									
	char mClassName[CLASSNAME_LENGTH];	
	RTTI* mBaseRTTI;						
	ClassFactoryFunc mClassFactory;					
	std::list<AbstractProperty*> mProperties;					
	ComponentDependencyList mComponentDependencies;

};

#endif //__RTTI_H__