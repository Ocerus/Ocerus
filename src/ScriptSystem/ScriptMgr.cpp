#include "Common.h"
#include "ScriptMgr.h"
#include "ScriptResource.h"

using namespace ScriptSystem;

struct TimeOut
{
	Utils::Timer timer;
	uint32 timeOut;

	TimeOut(uint32 to) : timeOut(to) {};
	bool IsTimeOut() { return timer.GetMilliseconds() > timeOut; }
	void Reset() { timer.Reset(); }
};

void MessageCallback(const asSMessageInfo* msg, void* param)
{
	const char* messageType[] = {"ERROR", "WARNING", "INFO"};

	ocError << msg->section << "(" << msg->row << ", " << msg->col << ") : "
		<< messageType[msg->type] << ": " << msg->message;
}

void LineCallback(asIScriptContext* ctx, TimeOut* timeOut)
{
	// If the time out is reached we suspend the script
	if (timeOut->IsTimeOut()) ctx->Suspend();
}

void ScriptLog(string& msg)
{
	ocInfo << msg;
}

ScriptMgr::ScriptMgr(const string& basepath)
{
	ocInfo << "*** ScriptMgr init ***";
	mBasePath = basepath;

	// Create the script engine
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	OC_ASSERT_MSG(engine, "Failed to create script engine.");

	// The script compiler will send any compiler messages to the callback function
	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	// Add functions and variables that can be called from script
	ConfigureEngine();
}

ScriptMgr::~ScriptMgr(void)
{
	ocInfo << "*** ScriptMgr deinit ***";
	/*UnloadData();
	ocInfo << "*** ScriptMgr unloaded ***";*/
}

template<class T>
T EntityHandleGetValue(EntitySystem::EntityHandle& handle, string& propName)
{
	return gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ).GetValue<T>();
}

template<class T>
void EntityHandleSetValue(EntitySystem::EntityHandle& handle, string& propName, T value)
{
	gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE).SetValue<T>(value);
}

void ScriptMgr::ConfigureEngine(void)
{
	int32 r;
    const char* msg = "Class, function or variable cannot be registered to script engine."; // Assert message

	// Register the script string type
	RegisterScriptString(engine);

	// Register log function
	r = engine->RegisterGlobalFunction("void Log(string &in)", asFUNCTION(ScriptLog), asCALL_CDECL);
		OC_ASSERT_MSG(r >= 0, msg);

	// Register EntityHandle class and it's methods
	r = engine->RegisterObjectType("EntityHandle", sizeof(EntitySystem::EntityHandle),
		asOBJ_VALUE | asOBJ_POD); OC_ASSERT_MSG(r >= 0, msg); // Register class EntityHandle as value

	r = engine->RegisterEnum("eEntityType"); OC_ASSERT_MSG(r>=0, msg); // Register enum eEntityType
	for (int32 entityType = 0; entityType<EntitySystem::NUM_ENTITY_TYPES; ++entityType)
	{
		r = engine->RegisterEnumValue("eEntityType", EntitySystem::EntityTypeNames[entityType], entityType); OC_ASSERT_MSG(r >= 0, msg);
	}
	r = engine->RegisterObjectMethod("EntityHandle", "eEntityType GetType() const",
		asMETHOD(EntitySystem::EntityHandle, GetType), asCALL_THISCALL); OC_ASSERT_MSG(r >= 0, msg); // Register EntityHandle::GetType()

	// Register getters and setters for supported types of properties
    #define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) \
	r = engine->RegisterObjectMethod("EntityHandle", (string(typeName) + " Get_" + typeName + "(string &in)").c_str(), \
		asFUNCTIONPR(EntityHandleGetValue, (EntitySystem::EntityHandle&, string&), typeClass), \
		asCALL_CDECL_OBJFIRST); OC_ASSERT_MSG(r >= 0, msg); \
	r = engine->RegisterObjectMethod("EntityHandle", (string("void Set_") + typeName + "(string &in, " + typeName + ")").c_str(), \
		asFUNCTIONPR(EntityHandleSetValue, (EntitySystem::EntityHandle&, string&, typeClass), void), \
		asCALL_CDECL_OBJFIRST); OC_ASSERT_MSG(r >= 0, msg);
	#define SCRIPT_ONLY
	#include "../Utils/Properties/PropertyTypes.h"
	#undef SCRIPT_ONLY
	#undef PROPERTY_TYPE
}

asIScriptContext* ScriptMgr::PrepareContext(const char* moduleName, const char* funcDecl)
{
	// Get module by name
	asIScriptModule* mod = GetModule(moduleName);
	if (mod == 0)
	{
		ocError << "Script module '" << moduleName << "' not found!";
		return 0;
	}

	// Get function ID from declaration
	int funcId = mod->GetFunctionIdByDecl(funcDecl);
	if (funcId < 0)
	{
		ocError << "Script function '" << funcDecl << "' not found/multiple found in module '"
			<< moduleName << "' or module was not compiled successfully!";
		return 0;
	}

	// Create context
	asIScriptContext* ctx = engine->CreateContext();
	OC_ASSERT_MSG(ctx, "Failed to create the script context.");

	// Prepare function on context
	int r = ctx->Prepare(funcId);
	OC_ASSERT_MSG(r >= 0, "Failed to prepare the script function.");

	return ctx;
}

bool ScriptMgr::ExecuteContext(asIScriptContext* ctx, uint32 timeOut)
{
	OC_ASSERT_MSG(ctx, "Cannot execute null context!");
	OC_ASSERT_MSG(ctx->GetState() == asEXECUTION_PREPARED, "Cannot execute unprepared context!");

	int funcId = ctx->GetCurrentFunction();
	const asIScriptFunction *function = engine->GetFunctionDescriptorById(funcId);
	const char* funcDecl = function->GetDeclaration();
	const char* moduleName = function->GetModuleName();
	int r;

	ocDebug << "Executing script function '" << funcDecl << "' in module '" << moduleName << "'.";

	// Set line callback function to avoid script cycling
	TimeOut* to = 0;
	if (timeOut != 0)
	{
		to = new TimeOut(timeOut);
		r = ctx->SetLineCallback(asFUNCTION(LineCallback), to, asCALL_CDECL);
		OC_ASSERT_MSG(r >= 0, "Failed to register line callback function.");
	}

	// Reset timer and execute script function
	if (to) to->Reset();
	r = ctx->Execute();
	if (to) delete to;

	// Get result of execution
	switch (r)
	{
	case asEXECUTION_ABORTED:  // Script was aborted by another thread.
		ocError << "Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' was aborted.";
		return false;
	case asEXECUTION_SUSPENDED: // Script was suspended due to time out.
		ocError << "Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' was suspended due to time out.";
		ctx->Abort();
		return false;
	case asEXECUTION_FINISHED: // Script was completed successfully
		ocDebug << "Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' completed successfully";
		return true;
	case asEXECUTION_EXCEPTION: // Exception occured in the script
		ocError << "Exception '" << ctx->GetExceptionString() << "' at line " <<
			ctx->GetExceptionLineNumber() << " in function '" << funcDecl <<
			"' in module '" << moduleName << "'!";
		return false;
	default:
		OC_NOT_REACHED();
		return false;
	}
}

asIScriptModule* ScriptMgr::GetModule(const char* fileName)
{
	// Get existing module
	asIScriptModule* mod = engine->GetModule(fileName, asGM_ONLY_IF_EXISTS);
	if (mod != 0) return mod;

	// Try to get existing script resource
	ScriptResourcePtr sp = boost::static_pointer_cast<ScriptResource>(gResourceMgr.GetResource("scripts", fileName));
	if (sp == 0)
	{
		// Load script resource from file
		gResourceMgr.AddResourceFileToGroup(mBasePath + fileName, "scripts",
			ResourceSystem::RESTYPE_SCRIPTRESOURCE, true);
		sp = boost::static_pointer_cast<ScriptResource>(gResourceMgr.GetResource("scripts", fileName));
	}
	if (sp == 0) return 0;

	mod = engine->GetModule(fileName, asGM_ALWAYS_CREATE);

	// Get script data from resource and add as script section
	const char* script = sp->GetScript();
	int r = mod->AddScriptSection(fileName, script);
	OC_ASSERT_MSG(r >= 0, "Failed to add script section.");
	sp->Unload();

	// Build module
	r = mod->Build();
	if (r < 0)
	{
		ocError << "Failed to build module '" << fileName << "'!";
		return 0;
	}

	return mod;
}