/// @file
/// Script system entry point.

#ifndef ScriptMgr_h__
#define ScriptMgr_h__

#include "Base.h"
#include "Singleton.h"
#include <angelscript.h>

/// Macro for easier use
#define gScriptMgr ScriptSystem::ScriptMgr::GetSingleton()

/// Scipt system allows users to define behaviour of components and entities
namespace ScriptSystem
{
	/// This class encapsulates script engine and manages access to script modules
	class ScriptMgr : public Singleton<ScriptMgr>
	{
	public:
		/// If the basepath parameter is provided, the manager will relate all files to this path.
		/// Global root is in the ResourceMgr's basepath and this basepath is relative to it.
		ScriptMgr(const string& basepath = "scripts/");

		~ScriptMgr(void);

		/// Returns new context prepared for passing the argument values.
		///	@param modulName Name of file where the main function of module is.
		/// @param funcDecl Declaration of function to be called in the script.
		asIScriptContext* PrepareContext(const char* moduleName, const char* funcDecl);

		/// Executes prepared context with specific time out. Don't forger to release context.
		/// @param ctx Prepared context with function arguments passed
		/// @param timeOut Time in ms after the execution of script will be aborted
		/// @return True if execution is successful (can get return value) 
		bool ExecuteContext(asIScriptContext* ctx, uint32 timeOut = 0);
	protected:
		/// Get script module represented by the name of file where the main function is.
		/// This function loads and builds module if necessary.
		asIScriptModule* GetModule(const char* fileName);
	private:
		/// Pointer to script engine.
		asIScriptEngine* engine;
		
		/// Basepath for script data. The root is in the ResourceMgr's basepath.
		string mBasePath;

		/// Configure the script engine with all the functions and variables that the script should be able to use.
		void ConfigureEngine(void);
	};
}

#endif // ScriptMgr_h__