/// @file
/// Script system entry point.

#ifndef ScriptMgr_h__
#define ScriptMgr_h__

#include "Base.h"
#include "Singleton.h"

/// Macro for easier use
#define gScriptMgr ScriptSystem::ScriptMgr::GetSingleton()

namespace AngelScript
{
	class asIScriptEngine;
	class asIScriptModule;
	class asIScriptContext;
	class CScriptBuilder;
	class CContextMgr;
}

/// Scipt system allows users to compile and run simple programs during the run-time.
/// The main class is ScriptMgr, but the script support for entities is handled via the EntityComponents::Script class.
/// Currently the only supported script language is <a href="http://www.angelcode.com/angelscript/">AngelScript</a>.
/// The script files are loaded as common resources by ScriptMgr which compiles them into modules. The modules are
/// identified by they script names derived from the resource names.
/// For more informations take a look into the user guide.
namespace ScriptSystem
{
	/// Vector of pointers to ScriptResource
	typedef vector<ScriptResourcePtr> ScriptResourcePtrs;
	
	/// This class encapsulates script engine and manages access to script modules.
	class ScriptMgr : public Singleton<ScriptMgr>
	{
	public:

		/// If the basepath parameter is provided, the manager will relate all files to this path.
		/// Global root is in the ResourceMgr's basepath and this basepath is relative to it.
		ScriptMgr();

		/// Destructor.
		~ScriptMgr(void);

		/// Get script function ID from module name and function declaration.
		///	@param modulName Name of file where the main function of module is.
		/// @param funcDecl Declaration of function to be called in the script.
		/// @return Number greater than or equal to zero that is function ID, number less than zero for not found
		int32 GetFunctionID(const char* moduleName, const char* funcDecl);
		
		/// Get a module name where the script function of a specified ID occurs.
		/// @param funcId Script function ID.
		/// @return Module name of script function, 0 if function with the ID is not found.
		const char* GetFunctionModuleName(int32 funcId);
		
		/// Get a function declaration of a script function with a specified ID.
		/// @param funcId Script function ID.
		/// @return Appropriate function declaration, 0 if function with the ID is not found.
		const char* GetFunctionDeclaration(int32 funcId);

		/// Returns new context prepared for passing the argument values.
		///	@param funcId ID of function to prepare (can get from GetFunctionID)
		AngelScript::asIScriptContext* PrepareContext(int32 funcId);

		/// Sets an argument to a function being prepared to be called. The type and value of the argument
		/// is determined from PropertyFunctionParameter.
		/// @param ctx Prepared context
		/// @param parameterIndex Index of function parameter (zero based)
		/// @param parameter Function parameter in form of PropertyFunctionParameter
		/// @return True if set is successful
		bool SetFunctionArgument(AngelScript::asIScriptContext* ctx, 
			const uint32 parameterIndex, const Reflection::PropertyFunctionParameter& parameter);

		/// Executes prepared context with specific time out. Don't forger to release context.
		/// @param ctx Prepared context with function arguments passed
		/// @param timeOut Time in ms after the execution of script will be aborted
		/// @return True if the execution was successful (can get return value)
		bool ExecuteContext(AngelScript::asIScriptContext* ctx, uint32 timeOut = 0);

		/// Builds and executes a string as an implementation of a function without parameters
		/// in context of defined module.
		/// @param script The script string that will be executed
		/// @param moduleName The name of the module in which the string should be executed (default null)
		/// @return True if the building and the execution were successful
		bool ExecuteString(const char* script, const char* moduleName = 0);

		/// Add a pre-processor define for conditional compilation.
		void DefineWord(const char* word);

		/// Get script module represented by the name of file where the main function is.
		/// This function loads and builds module if necessary.
		AngelScript::asIScriptModule* GetModule(const char* fileName);

		/// Unload script module represented by the name of file where the main function is.
		/// All the function ID from this module got from GetFunctionID will be superseded.
		void UnloadModule(const char* fileName);

		/// Unload all previously loaded and builded modules and abort all contexts in context manager.
		/// All the function ID got from GetFunctionID will be superseded as well as the contexts got from
		/// PrepareContext, AddContextToManager and AddContextAsCoRoutineToManager.
		void ClearModules();

		/// Returns the current time the script manager is using.
		uint64 GetTime() const;

		/// Returns true if the command/script is executed from the GUI console.
		inline bool IsExecutedFromConsole() const { return mExecFromConsole; }

	private:

		/// Pointer to script engine.
		AngelScript::asIScriptEngine* mEngine;

		/// Object that helps building scripts.
		AngelScript::CScriptBuilder* mScriptBuilder;

		/// Depencency of loaded modules to resources
		map<string, ScriptResourcePtrs> mModules;

		/// True if the engine is executing commands directly from the GUI console.
		bool mExecFromConsole;

		/// Configure the script engine with all the functions and variables that the script should be able to use.
		void ConfigureEngine(void);

		/// Callback for including files to module.
		static int IncludeCallback(const char* fileName, const char* from, AngelScript::CScriptBuilder* builder, void* userParam);
	};
}

int IncludeCallback(const char* fileName, const char* from, AngelScript::CScriptBuilder* builder, void* basePath);
EntitySystem::EntityHandle GetCurrentEntityHandle(void);

#endif // ScriptMgr_h__