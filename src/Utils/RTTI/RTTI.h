/// @file
/// RTTI data storage for RTTI enabled classes.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef __RTTI_H__
#define __RTTI_H__

#include "Base.h"
#include "Properties/PropertyAccess.h"

/// A set of classes implementing custom %RTTI and reflection.
/// The word 'reflection' means that classes are aware of what they are. They can generate their string name
/// in run-time and create instances of themselves based only on their string name. A system of generic access to class'es
/// properties (member fields hidden behind getters and setters) is present as well.
/// @remarks
/// To make use of %RTTI the custom class C having a predecessor P must extend RTTIGlue<C, P>. If C has no predecessor,
/// RTTIBaseClass must be used for P instead. The custom class then has to declare and define the RegisterReflection function which will
/// be automatically called during the program startup. In this function C can register its properties
/// and functions using the RTTIGlue::RegisterProperty and RTTIGlue::RegisterFunction functions.
namespace Reflection
{
	/// A list of generic properties.
	typedef vector<AbstractProperty*> AbstractPropertyList;

	/// Type of the identifier of a class.
	typedef uint32 ClassID;

	/// A factory function used for creating an instance of this class.
	/// The function must return a pointer to the base class to make use of the polymorphism.
	typedef RTTIBaseClass* (*ClassFactoryFunc)();

	/// This functions is called on client classes to register properties into the RTTI.
	/// Note that the RegisterReflection function is called automatically when the program boots up.
	/// This is handled by a trick in the RTTIGlue.h file (bottom).
	typedef bool (*RegisterReflectionFunc)();

	/// A list of entity component types determining dependencies of a component on other components.
	typedef vector<EntitySystem::eComponentType> ComponentDependencyList;

	/// A maximum length for a name of a class.
	const uint32 CLASSNAME_LENGTH = 48;

	/// The %RTTI description structure.
	class RTTI
	{
	public:

		/// RTTI constructor.
		/// The first paramter is a stub. I am not sure why this stub is necessary - removing ths stub will
		/// confuse the .NET compiler and produce compile errors with subsequent parameters. If anybody knows 
		/// why this is so, feel free to e-mail me at dfilion@hotmail.com
		/// @param dwStub Just a stub.
		/// @param CLID A unique class ID.
		/// @param szClassName The name of the class type this RTTI structure represents.
		/// @param pBaseClassRTTI The parent RTTI structure for this RTTI structure.
		/// @param pFactory A factory function callback for creating an instance of the bound class type.
		/// @param pReflectionFunc Callback called by the system to register the reflective properties.
		RTTI(uint8 dwStub, ClassID CLID, const char* szClassName, RTTI* pBaseClassRTTI, 
			ClassFactoryFunc pFactory, RegisterReflectionFunc pReflectionFunc);

		/// Fills a data structure with all properties of the represented class type, 
		/// including all ancestor types.
		void EnumProperties( AbstractPropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS );

		/// Fills a data structure with all properties of the represented class type, 
		/// including all ancestor types.
		void EnumProperties( RTTIBaseClass* owner, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS );

		/// Fills a data structure with all component dependencies of the represented class type, 
		/// including all ancestor types.
		void EnumComponentDependencies(ComponentDependencyList& out);

		/// Returns a property identified by it's string key. Access restriction filter can be defined.
		AbstractProperty* GetProperty(const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS);

		/// Adds a property to the RTTI.
		void AddProperty(AbstractProperty* prop);

		/// Adds a component dependency to the RTTI.
		/// A component can define that it depends on other components. This is then used to determine the order
		/// of creation of components. It is ensured that all components this component depends on will be created first.
		void AddComponentDependency(const EntitySystem::eComponentType dep);

		/// Returns true if the RTTI structure is of the type specified by CLID.
		inline bool	IsTypeOf(ClassID CLID) const { return mCLID == CLID; }

		/// Returns true if the RTTI structure is derived from the type specified by CLID.
		inline bool	IsDerivedFrom(ClassID CLID) const
		{
			if( mCLID == CLID )
				return true;
			else if( mBaseRTTI )
				return mBaseRTTI->IsDerivedFrom( CLID );
			return false;
		}

		/// Returns the base class'es RTTI structure.
		inline RTTI* GetAncestorRTTI(void) const { mBaseRTTI; }

		/// Returns this class'es ID.
		inline ClassID GetCLID(void) const { return mCLID; }

		/// Returns the class name.
		inline const char* GetClassName(void) const { return mClassName; }

		/// Returns the class factory function.
		inline ClassFactoryFunc	GetClassFactory(void) const { return mClassFactory; }

	private:

		/// A map of properties of this RTTI.
		typedef hash_map<StringKey, AbstractProperty*> PropertyMap;

		ClassID	mCLID;									
		char mClassName[CLASSNAME_LENGTH];	
		RTTI* mBaseRTTI;						
		ClassFactoryFunc mClassFactory;					
		PropertyMap mProperties;					
		ComponentDependencyList mComponentDependencies;

	};
}

#endif //__RTTI_H__