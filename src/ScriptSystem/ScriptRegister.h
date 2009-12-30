/// @file
/// Additional objects, functions etc. to register to script engine.

#ifndef ScriptRegister_h__
#define ScriptRegister_h__

#include "Base.h"

namespace AngelScript
{
	class asIScriptEngine;
}

namespace ScriptSystem
{
	/// Registers utility classses and functions into the script system.
	void RegisterAllAdditions(AngelScript::asIScriptEngine* engine);

	/// Callback for reloading script resources. It is called by ScriptResource before it unloads itself.
	void ResourceUnloadCallback(ScriptResource* resource);
}

#endif // ScriptRegister_h__