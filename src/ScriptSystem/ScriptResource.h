/// @file
/// Implementation of a resource able to read files containing script.

#ifndef ScriptResource_h__
#define ScriptResource_h__

#include "Base.h"
#include "../ResourceSystem/Resource.h"

namespace ScriptSystem
{
	class ScriptResource;
	
	typedef void (*ScriptResourceReloadCallback)(ScriptResource* resource);
	
	/// This class represents a single script resource.
	/// Main purpose of the class is to load a script file, not to store the actual data. You should
	///	only use ScriptMgr to work with script resource, although its possible to load it directly with ResourceMgr.
	/// All data will be unload after first use from ScriptMgr.
	class ScriptResource : public ResourceSystem::Resource
	{
	public:

		virtual ~ScriptResource(void);

		/// Factory method.
		static ResourceSystem::ResourcePtr CreateMe(void);

		/// Returns script text.
		const char* GetScript();

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_SCRIPTRESOURCE; }

		/// Register the callback for reloading the script resource
		static void SetReloadCallback(ScriptResourceReloadCallback callback);

		/// Return dependence modules
		set<string>& GetDependentModules(void) { return mDependentModules; }

		/// Reloads the resource
		virtual void Reload(void);

	protected:

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);

	private:

		/// String with script implementation.
		string mScript;

		/// Dependent modules
		set<string> mDependentModules;

		/// Callback for reloading the script resource
		static ScriptResourceReloadCallback mReloadCallback;
	};
}

#endif