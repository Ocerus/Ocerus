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
	/// Register all additions in this file to script engine.
	void RegisterAllAdditions(AngelScript::asIScriptEngine* engine);

	/// Callback for reloading script resources.
	void ResourceReloadCallback(ScriptResource* resource);
}

#endif // ScriptRegister_h__