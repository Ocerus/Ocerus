#include "Common.h"
#include "ScriptMgr.h"
#include "ScriptResource.h"
#include <angelscript.h>
#include "AddOn/scriptbuilder.h"
#include "AddOn/scriptstring.h"
#include "AddOn/contextmgr.h"

using namespace ScriptSystem;
using namespace EntitySystem;
using namespace AngelScript;

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
	// If the time out is reached we abort the script
	if (timeOut->IsTimeOut()) ctx->Abort();
}

// Except non-const char* as last argument that means basepath for files, will be deleted in function
int IncludeCallback(const char* fileName, const char* from, CScriptBuilder* builder, void* basePath)
{
	// Try to get existing script resource
	ScriptResourcePtr sp = boost::static_pointer_cast<ScriptResource>(gResourceMgr.GetResource("scripts", fileName));
	if (!sp)
	{
		// Load script resource from file
		gResourceMgr.AddResourceFileToGroup(string((char*)basePath) + fileName, "scripts", 
			ResourceSystem::RESTYPE_SCRIPTRESOURCE, true);
		sp = boost::static_pointer_cast<ScriptResource>(gResourceMgr.GetResource("Scripts", fileName));
	}
	// Base path is allocated char*, so we must delete it
	delete[] (char*)basePath;
	if (!sp)
	{
		ocError << "Failed to load script file " << fileName << ".";
		return -1;
	}

	// Get script data from resource and add as script section
	const char* script = sp->GetScript();
	int r = builder->AddSectionFromMemory(script, fileName);
	sp->Unload();
	if (r < 0) ocError << "Failed to add script file " << fileName << " due to dependecy on unloadable file(s).";
	return r;
}

uint32 GetTime()
{
	static Utils::Timer timer;
	return (uint32)timer.GetMilliseconds();
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
	mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	OC_ASSERT_MSG(mEngine, "Failed to create script engine.");

	// The script compiler will send any compiler messages to the callback function
	mEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	// Create script builder and set include callback for it
	mScriptBuilder = new CScriptBuilder();
	mScriptBuilder->SetIncludeCallback(IncludeCallback, Utils::StringConverter::FromString<char*>(mBasePath));

	// Create context manager and set get time callback for it
	mContextMgr = new CContextMgr();
	mContextMgr->SetGetTimeCallback(GetTime);

	// Add functions and variables that can be called from script
	ConfigureEngine();
}

ScriptMgr::~ScriptMgr(void)
{
	ocInfo << "*** ScriptMgr deinit ***";
	delete mContextMgr;
	delete mScriptBuilder;
	mEngine->Release();
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

// Functions for register Vector2 to script

static void Vector2DefaultConstructor(Vector2* self)
{
	new(self) Vector2();
}

static void Vector2CopyConstructor(const Vector2& other, Vector2* self)
{
	new(self) Vector2(other.x, other.y);
}

static void Vector2InitConstructor(float32 x, float32 y, Vector2* self)
{
	new(self) Vector2(x,y);
}

void RegisterScriptVector2(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("Vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the object properties
	r = engine->RegisterObjectProperty("Vector2", "float x", offsetof(Vector2, x)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectProperty("Vector2", "float y", offsetof(Vector2, y)); OC_SCRIPT_ASSERT();

	// Register the constructors
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(Vector2DefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(const Vector2 &in)", asFUNCTION(Vector2CopyConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(float, float)",  asFUNCTION(Vector2InitConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the operator overloads
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(const Vector2 &in)", asMETHODPR(Vector2, operator+=, (const Vector2 &), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opSubAssign(const Vector2 &in)", asMETHODPR(Vector2, operator-=, (const Vector2 &), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opMulAssign(float)", asMETHODPR(Vector2, operator*=, (float32), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void &opAddAssign(const Vector2 &in)", asMETHODPR(Vector2, operator+=, (const Vector2 &), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void &opSubAssign(const Vector2 &in)", asMETHODPR(Vector2, operator-=, (const Vector2 &), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void &opMulAssign(float)", asMETHODPR(Vector2, operator*=, (float32), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opDivAssign(float)", asMETHODPR(Vector2, operator/=, (float32), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opNeg() const", asMETHOD(Vector2, operator-), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "bool opEquals(const Vector2 &in) const", asFUNCTIONPR(operator==, (const Vector2&, const Vector2&), bool), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const", asFUNCTIONPR(operator+, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const", asFUNCTIONPR(operator-, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(float) const", asFUNCTIONPR(operator*, (const Vector2&, float32), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul_r(float) const", asFUNCTIONPR(operator*, (float32, const Vector2&), Vector2), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 opDiv(float) const", asFUNCTIONPR(operator/, (const Vector2&, float32), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("Vector2", "float Length() const", asMETHOD(Vector2, Length), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float LengthSquared() const", asMETHOD(Vector2, LengthSquared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void Set(float, float)", asMETHODPR(Vector2, Set, (float32, float32), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void SetZero()", asMETHOD(Vector2, SetZero), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float Normalize()", asMETHOD(Vector2, Normalize), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "bool IsValid() const", asMETHOD(Vector2, IsValid), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float Dot(const Vector2 &in) const", asFUNCTION(b2Dot), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
}

// Functions for register StringKey to script

static void StringKeyDefaultConstructor(StringKey* self)
{
	new(self) StringKey();
}

static void StringKeyCopyConstructor(const StringKey& other, StringKey* self)
{
	new(self) StringKey(other);
}

static void StringKeyInitConstructor(const string& str, Vector2* self)
{
	new(self) StringKey(str);
}

static void StringKeyDestructor(StringKey* self)
{
	self->~StringKey();
}

void RegisterScriptStringKey(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("StringKey", sizeof(StringKey), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(StringKeyDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_CONSTRUCT, "void f(const StringKey &in)", asFUNCTION(StringKeyCopyConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(StringKeyInitConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(StringKeyDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the operator overloads
	r = engine->RegisterObjectMethod("StringKey", "bool opEquals(const StringKey &in) const", asMETHOD(StringKey, operator==), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("StringKey", "StringKey& opAssign(const StringKey &in)", asMETHOD(StringKey, operator=), asCALL_THISCALL); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("StringKey", "string ToString() const", asMETHOD(StringKey, ToString), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

// Functions for register EntityHandle to script

static void EntityHandleDefaultConstructor(EntityHandle* self)
{
	new(self) EntityHandle();
}

static void EntityHandleCopyConstructor(const EntityHandle& other, EntityHandle* self)
{
	new(self) EntityHandle(other);
}

static void EntityHandleDestructor(EntityHandle* self)
{
	self->~EntityHandle();
}

void RegisterScriptEntityHandle(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("EntityHandle", sizeof(EntityHandle), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("EntityHandle", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(EntityHandleDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("EntityHandle", asBEHAVE_CONSTRUCT, "void f(const EntityHandle &in)", asFUNCTION(EntityHandleCopyConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("EntityHandle", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(EntityHandleDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the operator overloads
	r = engine->RegisterObjectMethod("EntityHandle", "bool opEquals(const EntityHandle &in) const", asMETHOD(EntityHandle, operator==), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "EntityHandle& opAssign(const EntityHandle &in)", asMETHOD(EntityHandle, operator=), asCALL_THISCALL); OC_SCRIPT_ASSERT();

	// Register enum eEntityType
	r = engine->RegisterEnum("eEntityType"); OC_SCRIPT_ASSERT();
	for (int32 entityType = 0; entityType<EntitySystem::NUM_ENTITY_TYPES; ++entityType)
	{
		r = engine->RegisterEnumValue("eEntityType", EntitySystem::EntityTypeNames[entityType], entityType); OC_SCRIPT_ASSERT();
	}
	
	// Register typedef EntityID
	r = engine->RegisterTypedef("EntityID", "int32");
	
	// Register the object methods
	r = engine->RegisterObjectMethod("EntityHandle", "bool IsValid() const", asMETHOD(EntityHandle, IsValid), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "bool Exists() const", asMETHOD(EntityHandle, Exists), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "eEntityType GetType() const", asMETHOD(EntityHandle, GetType), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "EntityID GetID() const", asMETHOD(EntityHandle, GetID), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

// Functions for register Color to script

static void ColorDefaultConstructor(GfxSystem::Color* self)
{
	new(self) GfxSystem::Color();
}


static void ColorInit4Constructor(uint8 r, uint8 g, uint8 b, uint8 a, GfxSystem::Color* self)
{
	new(self) GfxSystem::Color(r, g, b, a);
}

static void ColorInit3Constructor(uint8 r, uint8 g, uint8 b, GfxSystem::Color* self)
{
	new(self) GfxSystem::Color(r, g, b);
}

static void ColorInit1Constructor(uint32 color, GfxSystem::Color* self)
{
	new(self) GfxSystem::Color(color);
}

void RegisterScriptColor(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("Color", sizeof(GfxSystem::Color), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the object properties
	r = engine->RegisterObjectProperty("Color", "uint8 r", offsetof(GfxSystem::Color, r)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectProperty("Color", "uint8 g", offsetof(GfxSystem::Color, g)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectProperty("Color", "uint8 b", offsetof(GfxSystem::Color, b)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectProperty("Color", "uint8 a", offsetof(GfxSystem::Color, a)); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ColorDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint8, uint8, uint8, uint8)", asFUNCTION(ColorInit4Constructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint8, uint8, uint8)", asFUNCTION(ColorInit3Constructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint32)", asFUNCTION(ColorInit1Constructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	
	// Register the object methods
	r = engine->RegisterObjectMethod("Color", "uint32 GetARGB() const", asMETHOD(GfxSystem::Color, GetARGB), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

// Functions for creating co-routines

void ScriptCreateCoRoutine(string &func, EntityHandle arg)
{
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx)
	{
		asIScriptEngine *engine = ctx->GetEngine();
		string mod = engine->GetFunctionDescriptorById(ctx->GetCurrentFunction())->GetModuleName();

		// We need to find the function that will be created as the co-routine
		string decl = "void " + func + "(EntityHandle handle)"; 
		int32 funcId = engine->GetModule(mod.c_str())->GetFunctionIdByDecl(decl.c_str());
		if (funcId < 0)
		{
			// No function could be found, raise an exception
			ctx->SetException(("Function '" + decl + "' doesn't exist").c_str());
			return;
		}

		// Create a new context for the co-routine
		asIScriptContext *coctx = gScriptMgr.AddContextAsCoRoutineToManager(ctx, funcId);

		// Pass the argument to the context
		int32 r = coctx->SetArgObject(0, &arg);
		OC_ASSERT(r >= 0);
	}
}

void ScriptMgr::ConfigureEngine(void)
{
	int32 r;

	// Register the script string type
	RegisterStdString(mEngine);

	// Registers the script function "void sleep(uint milliseconds)"
	mContextMgr->RegisterThreadSupport(mEngine);

	// Registers the script function "void yield()"
	mContextMgr->RegisterCoRoutineSupport(mEngine);

	// Register log function
	r = mEngine->RegisterGlobalFunction("void Log(string &in)", asFUNCTION(ScriptLog), asCALL_CDECL); OC_SCRIPT_ASSERT();

	// Register Vector2 class and it's methods
	RegisterScriptVector2(mEngine);

	// Register StringKey class and it's methods
	RegisterScriptStringKey(mEngine);
		
    // Register EntityHandle class and it's methods
	RegisterScriptEntityHandle(mEngine);

	// Register Color struct and it's methods
	RegisterScriptColor(mEngine);

	// Register function for creating co-routine
	r = mEngine->RegisterGlobalFunction("void createCoRoutine(const string &in, EntityHandle handle)", 
		asFUNCTIONPR(ScriptCreateCoRoutine, (string&, EntityHandle), void), asCALL_CDECL); OC_SCRIPT_ASSERT();

	// Register getters and setters for supported types of properties
    #define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string(typeName) + " Get_" + typeName + "(string &in)").c_str(), \
		asFUNCTIONPR(EntityHandleGetValue, (EntitySystem::EntityHandle&, string&), typeClass), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("void Set_") + typeName + "(string &in, " + typeName + ")").c_str(), \
		asFUNCTIONPR(EntityHandleSetValue, (EntitySystem::EntityHandle&, string&, typeClass), void), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	#define SCRIPT_ONLY
	#include "../Utils/Properties/PropertyTypes.h"
	#undef SCRIPT_ONLY
	#undef PROPERTY_TYPE
}

asIScriptContext* ScriptMgr::PrepareContext(int32 funcId)
{
	OC_ASSERT_MSG(funcId >= 0, "Invalid function ID passed.");

	// Create context
	asIScriptContext* ctx = mEngine->CreateContext();
	OC_ASSERT_MSG(ctx, "Failed to create the script context.");

	// Prepare function on context
	int32 r = ctx->Prepare(funcId);
	OC_ASSERT_MSG(r >= 0, "Failed to prepare the script function.");

	return ctx;
}

bool ScriptMgr::ExecuteContext(asIScriptContext* ctx, uint32 timeOut)
{
	OC_ASSERT_MSG(ctx, "Cannot execute null context!");
	OC_ASSERT_MSG(ctx->GetState() == asEXECUTION_PREPARED, "Cannot execute unprepared context!");

	int32 funcId = ctx->GetCurrentFunction();
	const asIScriptFunction *function = mEngine->GetFunctionDescriptorById(funcId);
	const char* funcDecl = function->GetDeclaration();
	const char* moduleName = function->GetModuleName();
	int32 r;

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
	case asEXECUTION_ABORTED:  // Script was aborted due to time out.
		ocError << "Execution of script function '" << funcDecl << "' in module '" << moduleName 
			<< "' was aborted due to time out.";
		return false;
	case asEXECUTION_SUSPENDED: // Script was suspended by itself. Caller can continue the script.
		ocDebug << "Execution of script function '" << funcDecl << "' in module '" << moduleName 
			<< "' was suspended.";
		return true;
	case asEXECUTION_FINISHED: // Script was completed successfully
		ocDebug << "Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' completed successfully.";
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

asIScriptContext* ScriptMgr::AddContextToManager(int32 funcId)
{
	OC_ASSERT_MSG(funcId >= 0, "Invalid function ID passed.");

	// Add context to context manager
	asIScriptContext* ctx = mContextMgr->AddContext(mEngine, funcId);
	OC_ASSERT_MSG(ctx, "Failed to create or prepare the script context.");

	return ctx;
}

asIScriptContext* ScriptMgr::AddContextAsCoRoutineToManager(asIScriptContext* currCtx, int32 funcId)
{
	OC_ASSERT_MSG(currCtx, "Parent context for co-routine must not be null.");
	OC_ASSERT_MSG(funcId >= 0, "Invalid function ID passed.");

	// Add context to context manager
	asIScriptContext* ctx = mContextMgr->AddContextForCoRoutine(currCtx, funcId);
	OC_ASSERT_MSG(ctx, "Failed to create or prepare the script context.");

	return ctx;
}

inline void ScriptMgr::ExecuteScripts()
{
	mContextMgr->ExecuteScripts();
}

asIScriptModule* ScriptMgr::GetModule(const char* fileName)
{
	// Get existing module
	asIScriptModule* mod = mEngine->GetModule(fileName, asGM_ONLY_IF_EXISTS);
	if (mod != 0) return mod;

	int32 r;
	// Create script builder to build new module
	r = mScriptBuilder->StartNewModule(mEngine, fileName);
	OC_ASSERT_MSG(r==0, "Failed to add module to script engine.");

	// Include main file
	r = IncludeCallback(fileName, "", mScriptBuilder, Utils::StringConverter::FromString<char*>(mBasePath));
	if (r < 0) return 0;

	// Build module
	r = mScriptBuilder->BuildModule();
	if (r < 0)
	{
		ocError << "Failed to build module '" << fileName << "'!";
		return 0;
	}

	return mEngine->GetModule(fileName, asGM_ONLY_IF_EXISTS);
}

int32 ScriptMgr::GetFunctionID(const char* moduleName, const char* funcDecl)
{
	// Get module by name
	asIScriptModule* mod = GetModule(moduleName);
	if (mod == 0)
	{
		ocError << "Script module '" << moduleName << "' not found!";
		return -1;
	}
	
	// Get function ID from declaration
	return mod->GetFunctionIdByDecl(funcDecl);
	/*if (funcId < 0)
	{
		ocError << "Script function '" << funcDecl << "' not found/multiple found in module '"
			<< moduleName << "' or module was not compiled successfully!";
		return -1;
	}

	return funcId;*/
}

void ScriptSystem::ScriptMgr::DefineWord( const char* word )
{
	mScriptBuilder->DefineWord(word);
}