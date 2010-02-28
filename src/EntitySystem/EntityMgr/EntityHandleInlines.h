/// @file
/// Implementation of inline functions of EntityHandle. These must be separated from the EntityHandle declaration to prevent cyclic dependencies.

#ifndef __ENTITYHANDLEINLINES_H__
#define __ENTITYHANDLEINLINES_H__


namespace EntitySystem
{
		template <class T>
		bool EntityHandle::RegisterDynamicPropertyOfComponent(const ComponentID component, 
			const StringKey propertyKey, const PropertyAccessFlags accessFlags, const string& comment)
        {
			return gEntityMgr.RegisterDynamicPropertyOfEntityComponent<T>(*this, component, propertyKey, accessFlags, comment);
		}
}


#endif // __ENTITYHANDLEINLINES_H__
