#include "Common.h"
#include "ScriptMgr.h"
#include "ScriptResource.h"
#include "ScriptRegister.h"
#include <angelscript.h>
#include "../Core/Application.h"
#include "AddOn/scriptbuilder.h"
#include "AddOn/scriptstring.h"
#include "AddOn/contextmgr.h"

using namespace ScriptSystem;
using namespace EntitySystem;
using namespace AngelScript;

void MessageCallback(const asSMessageInfo* msg, void* param)
{
	const char* messageType[] = {"ERROR", "WARNING", "INFO"};

	ocError << msg->section << "(" << msg->row << ", " << msg->col << ") : "
		<< messageType[msg->type] << ": " << msg->message;
}

void LineCallback(asIScriptContext* ctx, uint64* deadline)
{
	// If the time out is reached we abort the script
	if (*deadline <= gApp.GetCurrentTimeMillis()) ctx->Abort();
}

int ScriptMgr::IncludeCallback(const char* fileName, const char* from, AngelScript::CScriptBuilder* builder, void* userParam)
{
	// Try to get existing script resource
	ScriptResourcePtr sp = (ScriptResourcePtr)(gResourceMgr.GetResource("Scripts", fileName));
	if (!sp)
	{
		// Load script resource from file
		gResourceMgr.AddResourceFileToGroup(gScriptMgr.mBasePath + fileName, "Scripts",
			ResourceSystem::RESTYPE_SCRIPTRESOURCE, true);
		sp = (ScriptResourcePtr)(gResourceMgr.GetResource("Scripts", fileName));
	}
	if (!sp)
	{
		ocError << "Failed to load script file " << fileName << ".";
		return -1;
	}

	// Get script data from resource and add as script section
	const char* script = sp->GetScript();
	int r = builder->AddSectionFromMemory(script, fileName);

	// Add file as module dependency
	sp->GetDependentModules().insert(string(builder->GetModuleName()));
	gScriptMgr.mModules[string(builder->GetModuleName())].push_back(sp);

	if (r < 0) ocError << "Failed to add script file " << fileName << " due to dependecy on unloadable file(s).";
	return r;
}

uint32 GetTime()
{
	return (uint32)gApp.GetCurrentTimeMillis();
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
	mScriptBuilder->SetIncludeCallback(&ScriptMgr::IncludeCallback, 0);

	/*
	// Create context manager and set get time callback for it
	mContextMgr = new CContextMgr();
	mContextMgr->SetGetTimeCallback(GetTime);
	*/

	// Set script resource unload callback
	ScriptResource::SetUnloadCallback(&ResourceUnloadCallback);

	// Add functions and variables that can be called from script
	ConfigureEngine();
}

ScriptMgr::~ScriptMgr(void)
{
	ocInfo << "*** ScriptMgr deinit ***";
	/*delete mContextMgr;*/
	delete mScriptBuilder;
	mEngine->Release();
}

// Template function called from scripts that finds property (propName) of entity handle (handle) and gets its value.
template<typename T>
T EntityHandleGetValue(EntitySystem::EntityHandle& handle, string& propName)
{
	Reflection::PropertyHolder ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ);
	if (ph.IsValid()) return ph.GetValue<T>();
	else
	{
		asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
		return Reflection::PropertyTypes::GetDefaultValue<T>();
	}
}

// Template function called from scripts that finds property (propName) of entity handle (handle) and sets its value.
template<typename T>
void EntityHandleSetValue(EntitySystem::EntityHandle& handle, string& propName, T value)
{
	Reflection::PropertyHolder ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE);
	if (ph.IsValid()) ph.SetValue<T>(value);
	else
	{
		asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
	}
}

// Function called from scripts that finds property (propName) of entity handle (handle) and call it as function with parameters (value).
void EntityHandleCallFunction(EntitySystem::EntityHandle& handle, string& propName, Reflection::PropertyFunctionParameters& value)
{
	Reflection::PropertyHolder ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE);
	if (ph.IsValid()) ph.SetValue<PropertyFunctionParameters>(value);
	else
	{
		asGetActiveContext()->SetException(("Function '" + propName + "' does not exist or you don't have access rights!").c_str());
	}
}

// Template proxy class that wrap Utils::Array<T> for better communication with scripts.
template<typename T>
class ScriptArray
{
public:
	inline ScriptArray(Utils::Array<T>* array = 0) : mArray(array) {}

	// Read accessor to an array item.
	inline T operator[](const int32 index) const
	{
		if (!mArray) asGetActiveContext()->SetException("Used uninicialized array!");
		if (index>=0 && index<mArray->GetSize()) return (*mArray)[index];
		else
		{
			asGetActiveContext()->SetException("Array index out of bounds!");
			return PropertyTypes::GetDefaultValue<T>();
		}
	}

	// Write accessor to an array item.
	inline T& operator[](const int32 index)
	{
		if (!mArray) asGetActiveContext()->SetException("Used uninicialized array!");
		if (index>=0 && index<mArray->GetSize()) return (*mArray)[index];
		else
		{
			asGetActiveContext()->SetException("Array index out of bounds!");
			return (*mArray)[0];
		}
	}

	// Returns a size of the array
	inline int32 GetSize() const { return mArray->GetSize(); }

	// Resize an array to a newSize
	inline void Resize(int32 newSize)
	{
		if (newSize<0)
		{
			asGetActiveContext()->SetException("Cannot resize array to negative size!");
			return;
		}
		mArray->Resize(newSize);
	}
private:
	Utils::Array<T>* mArray;
};

// Template function called from scripts that finds array property (propName) of entity handle (handle) and returns it as non-const.
template<typename U>
ScriptArray<U> EntityHandleGetArrayValue(EntitySystem::EntityHandle& handle, string& propName)
{
	Reflection::PropertyHolder ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE);
	if (ph.IsValid()) return ScriptArray<U>(ph.GetValue<Array<U>*>());
	else
	{
		asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
		return ScriptArray<U>(0);
	}
}

// Template function called from scripts that finds array property (propName) of entity handle (handle) and returns it as const.
template<typename U>
const ScriptArray<U> EntityHandleGetConstArrayValue(EntitySystem::EntityHandle& handle, string& propName)
{
	Reflection::PropertyHolder ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ);
	if (ph.IsValid()) return ScriptArray<U>(ph.GetValue<Array<U>*>());
	else
	{
		asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
		return ScriptArray<U>(0);
	}
}

// Functions for register ScriptArray<U> to script

template<typename U>
static void ArrayDefaultConstructor(ScriptArray<U>* self)
{
	new(self) ScriptArray<U>();
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

// Functions for register PropertyFunctionParameters to script

static void PropertyFunctionParametersDefaultConstructor(Reflection::PropertyFunctionParameters* self)
{
	new(self) Reflection::PropertyFunctionParameters();
}

static void PropertyFunctionParametersDestructor(Reflection::PropertyFunctionParameters* self)
{
	self->~PropertyFunctionParameters();
}

template <typename T>
void PropertyFunctionParametersPushParameter(Reflection::PropertyFunctionParameters& pfp, const T& param)
{
	pfp.PushParameter(Reflection::PropertyFunctionParameter(param));
}

template <typename T>
Reflection::PropertyFunctionParameters PropertyFunctionParametersOperatorShl(Reflection::PropertyFunctionParameters& pfp, const T& param)
{
	return pfp << Reflection::PropertyFunctionParameter(param);
}

void RegisterScriptPropertyFunctionParameters(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("PropertyFunctionParameters", sizeof(Reflection::PropertyFunctionParameters),
		asOBJ_VALUE | asOBJ_APP_CLASS_CDA); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("PropertyFunctionParameters", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(PropertyFunctionParametersDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("PropertyFunctionParameters", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(PropertyFunctionParametersDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the operator overloads
	r = engine->RegisterObjectMethod("PropertyFunctionParameters", "bool opEquals(const PropertyFunctionParameters &in) const", asMETHOD(PropertyFunctionParameters, operator==), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("PropertyFunctionParameters", "PropertyFunctionParameters& opAssign(const PropertyFunctionParameters &in)", asMETHOD(PropertyFunctionParameters, operator=), asCALL_THISCALL); OC_SCRIPT_ASSERT();
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

	// Register enum eEntityMessageType
	r = engine->RegisterEnum("eEntityMessageType"); OC_SCRIPT_ASSERT();
	for (int32 entityMessageType = 0; entityMessageType<EntitySystem::EntityMessage::NUM_TYPES; ++entityMessageType)
	{
		r = engine->RegisterEnumValue("eEntityMessageType", EntitySystem::EntityMessage::GetTypeName((EntitySystem::EntityMessage::eType)entityMessageType), entityMessageType); OC_SCRIPT_ASSERT();
	}
	// Register enum eEntityMessageResult
	r = engine->RegisterEnum("eEntityMessageResult"); OC_SCRIPT_ASSERT();
	for (int32 entityMessageResult = 0; entityMessageResult<=EntitySystem::EntityMessage::RESULT_ERROR; ++entityMessageResult)
	{
		r = engine->RegisterEnumValue("eEntityMessageResult", EntitySystem::EntityMessage::GetResultName((EntitySystem::EntityMessage::eResult)entityMessageResult), entityMessageResult); OC_SCRIPT_ASSERT();
	}

	// Register typedef EntityID
	r = engine->RegisterTypedef("EntityID", "int32");

	// Register the object methods
	r = engine->RegisterObjectMethod("EntityHandle", "bool IsValid() const", asMETHOD(EntityHandle, IsValid), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "bool Exists() const", asMETHOD(EntityHandle, Exists), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "EntityID GetID() const", asMETHOD(EntityHandle, GetID), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "eEntityMessageResult PostMessage(const eEntityMessageType)",
		asMETHODPR(EntityHandle, PostMessage, (const EntityMessage::eType), EntityMessage::eResult), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "eEntityMessageResult PostMessage(const eEntityMessageType, PropertyFunctionParameters)",
		asMETHODPR(EntityHandle, PostMessage, (const EntityMessage::eType, Reflection::PropertyFunctionParameters), EntityMessage::eResult), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

// Functions for register EntityDescription to script

static void EntityDescriptionDefaultConstructor(EntityDescription* self)
{
	new(self) EntityDescription();
}

static void EntityDescriptionDestructor(EntityDescription* self)
{
	self->~EntityDescription();
}

void RegisterScriptEntityDescription(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("EntityDescription", sizeof(EntityDescription), asOBJ_VALUE | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("EntityDescription", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(EntityDescriptionDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("EntityDescription", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(EntityDescriptionDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register enum eComponentType
	r = engine->RegisterEnum("eComponentType"); OC_SCRIPT_ASSERT();
	for (int32 componentType = 0; componentType < EntitySystem::NUM_COMPONENT_TYPES; ++componentType)
	{
		r = engine->RegisterEnumValue("eComponentType", EntitySystem::ComponentTypeNames
			[componentType], componentType); OC_SCRIPT_ASSERT();
	}
	// Register enum eEntityType
	r = engine->RegisterEnum("eEntityDescriptionKind"); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eEntityDescriptionKind", "EK_ENTITY", EntitySystem::EntityDescription::EK_ENTITY); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eEntityDescriptionKind", "EK_PROTOTYPE", EntitySystem::EntityDescription::EK_PROTOTYPE); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eEntityDescriptionKind", "EK_PROTOTYPE_COPY", EntitySystem::EntityDescription::EK_PROTOTYPE_COPY); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("EntityDescription", "void Reset()", asMETHOD(EntityDescription, Reset), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityDescription", "void AddComponent(const eComponentType)", asMETHOD(EntityDescription, AddComponent), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityDescription", "void SetName(const string &in)", asMETHOD(EntityDescription, SetName), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityDescription", "void SetKind(const eEntityDescriptionKind)", asMETHOD(EntityDescription, SetKind), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityDescription", "void SetPrototype(const EntityHandle)",
		asMETHODPR(EntityDescription, SetPrototype, (const EntityHandle), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityDescription", "void SetPrototype(const EntityID)",
		asMETHODPR(EntityDescription, SetPrototype, (const EntityID), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityDescription", "void SetDesiredID(const EntityID)", asMETHOD(EntityDescription, SetDesiredID), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

// Functions for register EntityMgr to script

EntityMgr& GetEntityMgr()
{
	return gEntityMgr;
}

void RegisterScriptEntityMgr(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("EntityMgr", 0, asOBJ_REF | asOBJ_NOHANDLE); OC_SCRIPT_ASSERT();

	// Register enum ePropertyAccess
	r = engine->RegisterEnum("ePropertyAccess"); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_EDIT_READ", Reflection::PA_EDIT_READ); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_EDIT_WRITE", Reflection::PA_EDIT_WRITE); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_SCRIPT_READ", Reflection::PA_SCRIPT_READ); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_SCRIPT_WRITE", Reflection::PA_SCRIPT_WRITE); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_INIT", Reflection::PA_INIT); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_FULL_ACCESS", Reflection::PA_FULL_ACCESS); OC_SCRIPT_ASSERT();

	// Register typedef ComponentID, PropertyAccessFlags
	r = engine->RegisterTypedef("ComponentID", "int32");
	r = engine->RegisterTypedef("PropertyAccessFlags", "uint8");

	// Register the object methods
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle CreateEntity(EntityDescription &in)", asMETHOD(EntityMgr, CreateEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void DestroyEntity(const EntityHandle)", asMETHOD(EntityMgr, DestroyEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool EntityExists(const EntityHandle) const", asMETHOD(EntityMgr, EntityExists), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle FindFirstEntity(const string &in)", asMETHOD(EntityMgr, FindFirstEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool IsEntityInited(const EntityHandle) const", asMETHOD(EntityMgr, IsEntityInited), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool IsEntityPrototype(const EntityHandle) const", asMETHOD(EntityMgr, IsEntityPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void LinkEntityToPrototype(const EntityHandle, const EntityHandle)", asMETHOD(EntityMgr, LinkEntityToPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void UnlinkEntityFromPrototype(const EntityHandle)", asMETHOD(EntityMgr, UnlinkEntityFromPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool IsPrototypePropertyShared(const EntityHandle, const StringKey) const", asMETHOD(EntityMgr, IsPrototypePropertyShared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void SetPrototypePropertyShared(const EntityHandle, const StringKey)", asMETHOD(EntityMgr, SetPrototypePropertyShared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void SetPrototypePropertyNonShared(const EntityHandle, const StringKey)", asMETHOD(EntityMgr, SetPrototypePropertyNonShared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void UpdatePrototypeCopy(const EntityHandle)", asMETHOD(EntityMgr, UpdatePrototypeCopy), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void UpdatePrototypeInstances(const EntityHandle)", asMETHOD(EntityMgr, UpdatePrototypeInstances), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool HasEntityProperty(const EntityHandle, const StringKey, const PropertyAccessFlags) const", asMETHOD(EntityMgr, HasEntityProperty), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool HasEntityComponentProperty(const EntityHandle, const ComponentID, const StringKey, const PropertyAccessFlags) const", asMETHOD(EntityMgr, HasEntityComponentProperty), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void BroadcastMessage(const eEntityMessageType)", asMETHODPR(EntityMgr, BroadcastMessage, (const EntityMessage::eType), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void BroadcastMessage(const eEntityMessageType, PropertyFunctionParameters)", asMETHODPR(EntityMgr, BroadcastMessage, (const EntityMessage::eType, Reflection::PropertyFunctionParameters), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool HasEntityComponentOfType(const EntityHandle, const eComponentType)", asMETHOD(EntityMgr, HasEntityComponentOfType), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "int32 GetNumberOfEntityComponents(const EntityHandle) const", asMETHOD(EntityMgr, GetNumberOfEntityComponents), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "ComponentID AddComponentToEntity(const EntityHandle, const eComponentType)", asMETHOD(EntityMgr, AddComponentToEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void DestroyEntityComponent(const EntityHandle, const ComponentID)", asMETHOD(EntityMgr, DestroyEntityComponent), asCALL_THISCALL); OC_SCRIPT_ASSERT();

	// Register function that returns it
	r = engine->RegisterGlobalFunction("EntityMgr& GetEntityMgr()", asFUNCTION(GetEntityMgr), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

/*
// Functions for creating co-routines

void ScriptCreateCoRoutine(string &func)
{
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx)
	{
		asIScriptEngine *engine = ctx->GetEngine();
		string mod = engine->GetFunctionDescriptorById(ctx->GetCurrentFunction())->GetModuleName();

		// We need to find the function that will be created as the co-routine
		string decl = "void " + func + "()";
		int32 funcId = engine->GetModule(mod.c_str())->GetFunctionIdByDecl(decl.c_str());
		if (funcId < 0)
		{
			// No function could be found, raise an exception
			ctx->SetException(("Function '" + decl + "' doesn't exist").c_str());
			return;
		}

		// Create a new context for the co-routine
		//asIScriptContext *coctx = 
		gScriptMgr.AddContextAsCoRoutineToManager(ctx, funcId);

		// Pass the argument to the context
		//int32 r = coctx->SetArgObject(0, &arg);
		//OC_ASSERT(r >= 0);
	}
}*/

// Get handle of entity which ran the script
EntityHandle GetCurrentEntityHandle(void)
{
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx)
	{
		EntityHandle *handle = static_cast<EntityHandle*>(ctx->GetUserData());
		if (handle) return *handle;
	}
	return EntityHandle::Null;
}

// Get state of current OnAction handler
int32 ScriptGetCurrentState(void)
{
	string exception;
	// Find current entity handle
	EntityHandle handle = GetCurrentEntityHandle();
	if (handle.IsValid())
	{
		// Find current array index
		Reflection::PropertyHolder holder = handle.GetProperty("ScriptCurrentArrayIndex");
		if (holder.IsValid())
		{
			int32 index = holder.GetValue<int32>();
			Array<int32>* states = handle.GetProperty("ScriptStates").GetValue<Array<int32>*>();
			if (states != 0 && index >= 0 && states->GetSize() > index )
			{
				// Return current state
				return (*states)[index];
			} else exception = "This function must be called from OnAction handler.";
		} else exception = "This function must be called from entity with Script component.";
	} else exception = "This function must be called from an entity message handler.";
	
	// Solve exceptions
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx) ctx->SetException(exception.c_str());
	return 0;
}

void ScriptSetAndSleep(int32 state, uint64 time)
{
	string exception;
	// Find current entity handle
	EntityHandle handle = GetCurrentEntityHandle();
	if (handle.IsValid())
	{
		// Find current array index
		Reflection::PropertyHolder holder = handle.GetProperty("ScriptCurrentArrayIndex");
		if (holder.IsValid())
		{
			int32 index = holder.GetValue<int32>();
			Array<int32>* states = handle.GetProperty("ScriptStates").GetValue<Array<int32>*>();
			Array<uint64>* times = handle.GetProperty("ScriptTimes").GetValue<Array<uint64>*>();
			if (states != 0 && times != 0 && index >= 0 && states->GetSize() > index && times->GetSize() > index)
			{
				// Set state and time
				(*states)[index] = state;
				(*times)[index] = gApp.GetCurrentTimeMillis() + time;
				return;
			} else exception = "This function must be called from OnAction handler.";
		} else exception = "This function must be called from entity with Script component.";
	} else exception = "This function must be called from an entity message handler.";
	
	// Solve exceptions
	asIScriptContext *ctx = asGetActiveContext();
	if (ctx) ctx->SetException(exception.c_str());
}

void ScriptMgr::ConfigureEngine(void)
{
	int32 r;

	// Register typedefs for real numbers
	r = mEngine->RegisterTypedef("float32", "float"); OC_SCRIPT_ASSERT();
	r = mEngine->RegisterTypedef("float64", "double"); OC_SCRIPT_ASSERT();
	
	// Register the script string type
	RegisterStdString(mEngine);

	/*// Registers the script function "void sleep(uint milliseconds)"
	mContextMgr->RegisterThreadSupport(mEngine);

	// Registers the script function "void yield()"
	mContextMgr->RegisterCoRoutineSupport(mEngine);

	// Register function for creating co-routine
	r = mEngine->RegisterGlobalFunction("void createCoRoutine(const string &in)",
		asFUNCTIONPR(ScriptCreateCoRoutine, (string&), void), asCALL_CDECL); OC_SCRIPT_ASSERT();*/

	// Register functions for OnAction state and time of execution support
	r = mEngine->RegisterGlobalFunction("int32 GetState()", asFUNCTION(ScriptGetCurrentState), asCALL_CDECL); OC_SCRIPT_ASSERT();
	r = mEngine->RegisterGlobalFunction("void SetAndSleep(int32, uint64)", asFUNCTION(ScriptSetAndSleep), asCALL_CDECL); OC_SCRIPT_ASSERT();

	// Register log function
	r = mEngine->RegisterGlobalFunction("void Log(string &in)", asFUNCTION(ScriptLog), asCALL_CDECL); OC_SCRIPT_ASSERT();

	// Register StringKey class and it's methods
	RegisterScriptStringKey(mEngine);

	// Register PropertyFunctionParameter class and it's methods
	RegisterScriptPropertyFunctionParameters(mEngine);

    // Register EntityHandle class and it's methods
	RegisterScriptEntityHandle(mEngine);

	// Register EntityDescription class and it's methods
	RegisterScriptEntityDescription(mEngine);

	// Register EntityMgr class and it's methods
	RegisterScriptEntityMgr(mEngine);

	// Register a function for getting current owner entity handle
	r = mEngine->RegisterGlobalFunction("EntityHandle GetCurrentEntityHandle()",
		asFUNCTION(GetCurrentEntityHandle), asCALL_CDECL); OC_SCRIPT_ASSERT();

	// Register a call function on EntityHandle
	r = mEngine->RegisterObjectMethod("EntityHandle", "void CallFunction(string &in, PropertyFunctionParameters &in)",
		asFUNCTION(EntityHandleCallFunction), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

	// Register all additions in ScriptRegister.cpp to script
	RegisterAllAdditions(mEngine);

	// Register getters, setters and array for supported types of properties

    #define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) \
	/* Register getter and setter */ \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string(typeName) + " Get_" + typeName + "(string &in)").c_str(), \
		asFUNCTIONPR(EntityHandleGetValue, (EntitySystem::EntityHandle&, string&), typeClass), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("void Set_") + typeName + "(string &in, " + typeName + ")").c_str(), \
		asFUNCTIONPR(EntityHandleSetValue, (EntitySystem::EntityHandle&, string&, typeClass), void), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	/* Register the array type */ \
	r = mEngine->RegisterObjectType((string("array_") + typeName).c_str(), sizeof(ScriptArray<typeClass>), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT(); \
	/* Register the default (dummy) constructor of array type */ \
	r = mEngine->RegisterObjectBehaviour((string("array_") + typeName).c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ArrayDefaultConstructor<typeClass>), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT(); \
	/* Register the array methods */ \
	r = mEngine->RegisterObjectMethod((string("array_") + typeName).c_str(), "int32 GetSize() const", asMETHOD(ScriptArray<typeClass>, GetSize), asCALL_THISCALL); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod((string("array_") + typeName).c_str(), "void Resize(int32)", asMETHOD(ScriptArray<typeClass>, Resize), asCALL_THISCALL); OC_SCRIPT_ASSERT(); \
	/* Register both the const and non-const alternative for index behaviour of array */ \
	r = mEngine->RegisterObjectBehaviour((string("array_") + typeName).c_str(), asBEHAVE_INDEX, (string(typeName) + " &f(int32)").c_str(), asMETHODPR(ScriptArray<typeClass>, operator[], (int32), typeClass&), asCALL_THISCALL); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectBehaviour((string("array_") + typeName).c_str(), asBEHAVE_INDEX, (string(typeName) + " f(int32) const").c_str(), asMETHODPR(ScriptArray<typeClass>, operator[], (int32) const, typeClass), asCALL_THISCALL); OC_SCRIPT_ASSERT(); \
	/* Register array const and non-const getter */ \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("array_") + typeName + " Get_array_" + typeName + "(string &in)").c_str(), \
		asFUNCTIONPR(EntityHandleGetArrayValue, (EntitySystem::EntityHandle&, string&), ScriptArray<typeClass>), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("const array_") + typeName + " Get_const_array_" + typeName + "(string &in)").c_str(), \
		asFUNCTIONPR(EntityHandleGetConstArrayValue, (EntitySystem::EntityHandle&, string&), const ScriptArray<typeClass>), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	/* Register operator<< method for PropertyFunctionParameters */ \
	r = mEngine->RegisterObjectMethod("PropertyFunctionParameters", (string("PropertyFunctionParameters opShl(const ") + typeName + " &in) const").c_str(), \
		asFUNCTIONPR(PropertyFunctionParametersOperatorShl, (Reflection::PropertyFunctionParameters&, const typeClass&), Reflection::PropertyFunctionParameters), \
		asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("PropertyFunctionParameters", (string("PropertyFunctionParameters opShl(const array_") + typeName + " &in) const").c_str(), \
		asFUNCTIONPR(PropertyFunctionParametersOperatorShl, (Reflection::PropertyFunctionParameters&, const ScriptArray<typeClass>&), Reflection::PropertyFunctionParameters), \
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
	uint64 deadline = 0;
	if (timeOut != 0)
	{
		r = ctx->SetLineCallback(asFUNCTION(LineCallback), &deadline, asCALL_CDECL);
		OC_ASSERT_MSG(r >= 0, "Failed to register line callback function.");
		deadline = gApp.GetCurrentTimeMillis() + timeOut;
	}

	// Execute the script function and get result
	switch (ctx->Execute())
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

bool ScriptMgr::ExecuteString(const char* script, const char* moduleName)
{
	// Get the module by name
	asIScriptModule* mod = GetModule(moduleName);
	if (mod == 0)
	{
		ocError << "Script module '" << moduleName << "' not found!";
		return false;
	}

	// Execute the script string and get result
	switch(mEngine->ExecuteString(moduleName, script))
	{
		case asERROR: // failed to build
		case asBUILD_IN_PROGRESS:
		case asEXECUTION_ABORTED:
		case asEXECUTION_SUSPENDED:
		case asEXECUTION_EXCEPTION:
			return false;
		case asEXECUTION_FINISHED:
			return true;
		default:
			OC_NOT_REACHED();
			return false;
	}
}

/*asIScriptContext* ScriptMgr::AddContextToManager(int32 funcId)
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

void ScriptMgr::ExecuteScripts()
{
	mContextMgr->ExecuteScripts();
}*/

asIScriptModule* ScriptMgr::GetModule(const char* fileName)
{
	if (fileName == 0) return mEngine->GetModule(0, asGM_ALWAYS_CREATE);

	// Get existing module
	asIScriptModule* mod = mEngine->GetModule(fileName, asGM_ONLY_IF_EXISTS);
	if (mod != 0) return mod;

	int32 r;
	// Create script builder to build new module
	r = mScriptBuilder->StartNewModule(mEngine, fileName);
	OC_ASSERT_MSG(r==0, "Failed to add module to script engine.");
	mModules[string(fileName)] = ScriptResourcePtrs();

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

void ScriptMgr::DefineWord( const char* word )
{
	mScriptBuilder->DefineWord(word);
}

void ScriptMgr::UnloadModule(const char* fileName)
{
	/*mContextMgr->AbortAll();*/
	int32 r = mEngine->DiscardModule(fileName);
	if (r < 0) return;

	map<string, ScriptResourcePtrs>::iterator moduleIter;
	if ((moduleIter = mModules.find(string(fileName))) != mModules.end())
	{
		for (vector<ScriptResourcePtr>::iterator resourceIter = moduleIter->second.begin();
			resourceIter != moduleIter->second.end(); ++resourceIter)
		{
			(*resourceIter)->GetDependentModules().erase(string(fileName));
		}
		mModules.erase(moduleIter);
	}
}

void ScriptMgr::ClearModules()
{
	/*mContextMgr->AbortAll();*/
	map<string, ScriptResourcePtrs>::iterator iter;
	while ((iter = mModules.begin()) != mModules.end())
	{
		UnloadModule(iter->first.c_str());
	}
}

// These macros allow us to automatically define the setter function and its value type for the script function arguments.
#define ARGVALUE_Address(ptr) (void*)(ptr)
#define ARGVALUE_Object(ptr) (void*)(ptr)
#define ARGVALUE_Byte(ptr) (AngelScript::asBYTE)(*ptr)
#define ARGVALUE_Word(ptr) (AngelScript::asWORD)(*ptr)
#define ARGVALUE_DWord(ptr) (AngelScript::asDWORD)(*ptr)
#define ARGVALUE_QWord(ptr) (AngelScript::asQWORD)(*ptr)
#define ARGVALUE_Float(ptr) (float)(*ptr)
#define ARGVALUE_Double(ptr) (double)(*ptr)

bool ScriptMgr::SetFunctionArgument(AngelScript::asIScriptContext* ctx, const uint32 parameterIndex, 
									const Reflection::PropertyFunctionParameter& parameter)
{
	OC_ASSERT_MSG(ctx, "Cannot set function arguments to null context!");
	OC_ASSERT_MSG(ctx->GetState() == asEXECUTION_PREPARED, "Cannot set function arguments to unprepared context!");
	
	int errorCode = 0;

	switch (parameter.GetType())
	{
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) \
		case typeID: \
			errorCode = ctx->SetArg##scriptSetter(parameterIndex, ARGVALUE_##scriptSetter(parameter.GetData<typeClass>())); \
			break;
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE
	
	default:
		OC_ASSERT_MSG(false, "Unknown or unsupported property type");
	}

	if (errorCode != 0) {
		ocError << "Can't set script function argument: error code = " << errorCode;
		return false;
	}

	return true;
}