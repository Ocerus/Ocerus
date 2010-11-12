#include "Common.h"
#include "ScriptMgr.h"
#include "ScriptResource.h"
#include "ScriptRegister.h"
#include <angelscript.h>
#include "Core/Application.h"
#include "Core/Game.h"
#include "Core/Project.h"
#include "AddOn/scriptbuilder.h"
#include "AddOn/scriptstring.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/GUIConsole.h"
#include "GUISystem/GUIMgr.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"

using namespace ScriptSystem;
using namespace EntitySystem;
using namespace InputSystem;
using namespace GUISystem;
using namespace AngelScript;
using namespace Core;

void MessageCallback(const asSMessageInfo* msg, void* param)
{
	OC_UNUSED(param);
	const char* messageType[] = {"ERROR", "WARNING", "INFO"};

	if (gScriptMgr.IsExecutedFromConsole())
	{
		stringstream ss;
		ss << "Script: " << msg->message;
		gGUIMgr.GetConsole()->AppendScriptMessage(ss.str());
	}
	else
	{
		ocInfo << "Script: " << msg->section << "(" << msg->row << ", " << msg->col << ") : "
			<< messageType[msg->type] << ": " << msg->message;
	}
}

void LineCallback(asIScriptContext* ctx, uint64* deadline)
{
	// If the time out is reached we abort the script
	if (*deadline <= gScriptMgr.GetGlobalTime()) ctx->Abort();
}

int ScriptMgr::IncludeCallback(const char* fileName, const char* from, AngelScript::CScriptBuilder* builder, void* userParam)
{
	OC_UNUSED(from);
	OC_UNUSED(userParam);
	// Try to get existing script resource
	ScriptResourcePtr sp = gResourceMgr.GetResource("Project", fileName);
	if (!sp)
	{
		ocInfo << "Failed to load script file " << fileName << " included from " << from << ".";
		return -1;
	}

	// Get script data from resource and add as script section
	const char* script = sp->GetScript();
	int r = builder->AddSectionFromMemory(script, fileName);

	// Add file as module dependency
	sp->GetDependentModules().insert(string(builder->GetModuleName()));
	gScriptMgr.mModules[string(builder->GetModuleName())].push_back(sp);

	if (r < 0) ocInfo << "Failed to add script file " << fileName << " due to dependecy on unloadable file(s).";
	return r;
}

// Printing all types from script

template<typename T>
void ScriptPrintln(const T& msg)
{
	ScriptPrintln(Utils::StringConverter::ToString<T>(msg));
}

void ScriptPrintln(const string& msg)
{
	if (gScriptMgr.IsExecutedFromConsole())
	{
		stringstream ss;
		ss << "Script:" << msg;
		gGUIMgr.GetConsole()->AppendScriptMessage(ss.str());
	}
	else
	{
		ocInfo << "Script: " << msg;
	}
}

template<typename T>
string& AssignToString(const T& value, string& self)
{
	self = Utils::StringConverter::ToString<T>(value);
	return self;
}

template<typename T>
string& AddAssignToString(const T& value, string& self)
{
	self += Utils::StringConverter::ToString<T>(value);
	return self;
}

template<typename T>
string AddStringType(string& self, const T& value)
{
	return self + Utils::StringConverter::ToString<T>(value);
}

template<typename T>
string AddTypeString(const T& value, string& self)
{
	return Utils::StringConverter::ToString<T>(value) + self;
}

ScriptMgr::ScriptMgr()
{
	ocInfo << "*** ScriptMgr init ***";

	mExecFromConsole = false;

	// Create the script engine
	mEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	OC_ASSERT_MSG(mEngine, "Failed to create script engine.");

	// The script compiler will send any compiler messages to the callback function
	mEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	// Create script builder and set include callback for it
	mScriptBuilder = new CScriptBuilder();
	mScriptBuilder->SetIncludeCallback(&ScriptMgr::IncludeCallback, 0);

	// Set script resource unload callback
	ScriptResource::SetUnloadCallback(&ResourceUnloadCallback);

	// Add functions and variables that can be called from script
	ConfigureEngine();
}

ScriptMgr::~ScriptMgr(void)
{
	ocInfo << "*** ScriptMgr deinit ***";
	delete mScriptBuilder;
	mEngine->Release();
}

// Template function called from scripts that finds property (propName) of entity handle (handle) and gets its value.
template<typename T>
T EntityHandleGetValue(EntitySystem::EntityHandle& handle, const string& propName)
{
	if (handle.Exists())
	{
		Reflection::PropertyHolder ph;
		if (gEntityMgr.HasEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ))
		{
			ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ);
		}
		if (ph.IsValid())
		{
			if (ph.GetType() == Reflection::PropertyTypes::GetTypeID<T>())
			{
				return ph.GetValue<T>();
			}
			else
			{
				asGetActiveContext()->SetException(("Can't convert property '" + propName + "' from '" + 
					Reflection::PropertyTypes::GetStringName(ph.GetType()) + "' to '" +
					Reflection::PropertyTypes::GetStringName(Reflection::PropertyTypes::GetTypeID<T>()) + "'").c_str());
				return Reflection::PropertyTypes::GetDefaultValue<T>();
			}
		}
		else
		{
			asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
			return Reflection::PropertyTypes::GetDefaultValue<T>();
		}
	}
	else
	{
		asGetActiveContext()->SetException("Invalid entity handle!");
		return Reflection::PropertyTypes::GetDefaultValue<T>();
	}
}

// Template function called from scripts that finds property (propName) of entity handle (handle) and sets its value.
template<typename T>
void EntityHandleSetValue(EntitySystem::EntityHandle& handle, const string& propName, const T& value)
{
	if (handle.Exists())
	{
		Reflection::PropertyHolder ph;
		if (gEntityMgr.HasEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE))
		{
			ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE);
		}
		if (ph.IsValid())
		{
			if (ph.GetType() == Reflection::PropertyTypes::GetTypeID<T>())
			{
				ph.SetValue<T>(value);
			}
			else
			{
				asGetActiveContext()->SetException(("Can't convert property '" + propName + "' from '" + 
					Reflection::PropertyTypes::GetStringName(ph.GetType()) + "' to '" +
					Reflection::PropertyTypes::GetStringName(Reflection::PropertyTypes::GetTypeID<T>()) + "'").c_str());
			}
		}
		else
		{
			asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
		}
	}
	else
	{
		asGetActiveContext()->SetException("Invalid entity handle!");
	}
}

// Function called from scripts that finds property (propName) of entity handle (handle) and call it as function with parameters (value).
void EntityHandleCallFunction(EntitySystem::EntityHandle& handle, string& propName, Reflection::PropertyFunctionParameters& value)
{
	if (handle.Exists())
	{
		Reflection::PropertyHolder ph;
		if (gEntityMgr.HasEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE))
		{
			ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE);
		}
		if (ph.IsValid())
		{
			if (ph.GetType() == Reflection::PropertyTypes::GetTypeID<PropertyFunctionParameters>())
			{
				ph.SetValue<PropertyFunctionParameters>(value);
			}
			else
			{
				asGetActiveContext()->SetException(("Can't convert property '" + propName + "' from '" + 
					Reflection::PropertyTypes::GetStringName(ph.GetType()) + "' to '" +
					Reflection::PropertyTypes::GetStringName(Reflection::PropertyTypes::GetTypeID<PropertyFunctionParameters>()) + "'").c_str());
			}
		}
		else
		{
			asGetActiveContext()->SetException(("Function '" + propName + "' does not exist or you don't have access rights!").c_str());
		}
	}
	else
	{
		asGetActiveContext()->SetException("Invalid entity handle!");
	}
}

// Template proxy class that wrap Utils::Array<T> for better communication with scripts.
template<typename T>
class ScriptArray
{
public:
	inline ScriptArray(Utils::Array<T>* array = 0) : mArray(array) {}

	~ScriptArray() {}

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
	int32 GetSize() const
	{
		if (!mArray) return 0;
		return mArray->GetSize();
	}

	// Resize an array to a newSize
	inline void Resize(int32 newSize)
	{
		if (newSize<0)
		{
			asGetActiveContext()->SetException("Cannot resize array to negative size!");
			return;
		}
		if (!mArray)
		{
			asGetActiveContext()->SetException("Used uninicialized array!");
		}
		else
		{
			mArray->Resize(newSize);
		}
	}
private:
	Utils::Array<T>* mArray;
};

// Template function called from scripts that finds array property (propName) of entity handle (handle) and returns it as non-const.
template<typename U>
ScriptArray<U> EntityHandleGetArrayValue(EntitySystem::EntityHandle& handle, string& propName)
{
	if (handle.Exists())
	{
		Reflection::PropertyHolder ph;
		if (gEntityMgr.HasEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE))
		{
			ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_WRITE);
		}
		if (ph.IsValid())
		{
			if (ph.GetType() == Reflection::PropertyTypes::GetTypeID<Array<U>*>())
			{
				return ScriptArray<U>(ph.GetValue<Array<U>*>());
			}
			else
			{
				asGetActiveContext()->SetException(("Can't convert property '" + propName + "' from '" + 
					Reflection::PropertyTypes::GetStringName(ph.GetType()) + "' to '" +
					Reflection::PropertyTypes::GetStringName(Reflection::PropertyTypes::GetTypeID<Array<U>*>()) + "'").c_str());
				return ScriptArray<U>(0);
			}
		}
		else
		{
			asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
			return ScriptArray<U>(0);
		}
	}
	else
	{
		asGetActiveContext()->SetException("Invalid entity handle!");
		return ScriptArray<U>(0);
	}
}

// Template function called from scripts that finds array property (propName) of entity handle (handle) and returns it as const.
template<typename U>
const ScriptArray<U> EntityHandleGetConstArrayValue(EntitySystem::EntityHandle& handle, string& propName)
{
	if (handle.Exists())
	{
		Reflection::PropertyHolder ph;
		if (gEntityMgr.HasEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ))
		{
			ph = gEntityMgr.GetEntityProperty(handle, StringKey(propName), Reflection::PA_SCRIPT_READ);
		}
		if (ph.IsValid())
		{
			if (ph.GetType() == Reflection::PropertyTypes::GetTypeID<Array<U>*>())
			{
				return ScriptArray<U>(ph.GetValue<Array<U>*>());
			}
			else
			{
				asGetActiveContext()->SetException(("Can't convert property '" + propName + "' from '" + 
					Reflection::PropertyTypes::GetStringName(ph.GetType()) + "' to '" +
					Reflection::PropertyTypes::GetStringName(Reflection::PropertyTypes::GetTypeID<Array<U>*>()) + "'").c_str());
				return ScriptArray<U>(0);
			}
		}
		else
		{
			asGetActiveContext()->SetException(("Property '" + propName + "' does not exist or you don't have access rights!").c_str());
			return ScriptArray<U>(0);
		}
	}
	else
	{
		asGetActiveContext()->SetException("Invalid entity handle!");
		return ScriptArray<U>(0);
	}
}


// Template function called from scripts that finds array property (propName) of Game.
template<typename U>
ScriptArray<U> GameGetArrayValue(Core::Game& handle, string& propName)
{
	if (!handle.HasDynamicProperty(propName))
	{
		handle.SetDynamicProperty(propName, new Array<U>());
	}

	return ScriptArray<U>(handle.GetDynamicProperty<Array<U>*>(propName));
}

// Template function called from scripts that finds array property (propName) of Game.
template<typename U>
const ScriptArray<U> GameGetConstArrayValue(Core::Game& handle, string& propName)
{
	return GameGetArrayValue<U>(handle, propName);
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

StringKey StringToStringKey(const string* self)
{
	return StringKey(*self);
}

void RegisterScriptStringKey(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("StringKey", sizeof(StringKey), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(StringKeyDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_CONSTRUCT, "void f(const StringKey &in)", asFUNCTION(StringKeyCopyConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(StringKeyInitConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("StringKey", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(StringKeyDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_IMPLICIT_VALUE_CAST, "StringKey f() const", asFUNCTION(StringToStringKey), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

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

template <class T>
bool RegisterDynamicProperty(const EntityHandle& self,
							 const string& propertyName, const PropertyAccessFlags accessFlags, const string& comment)
{
	ComponentID id = gEntityMgr.GetEntityComponent(self, CT_Script);
	if (id == -1)
	{
		ocWarning << "Dynamic property " << propertyName << " cannot be registered because the entity '"
			<< self.GetName() << "' (" << self.GetID() << ") has not the Script component.";
		return false;
	}
	return self.RegisterDynamicPropertyOfComponent<T>(id, StringKey(propertyName), accessFlags, comment);
}

bool UnregisterDynamicProperty(const EntityHandle& self, const string& propertyName)
{
	ComponentID id = gEntityMgr.GetEntityComponent(self, CT_Script);
	if (id == -1) return false;
	return self.UnregisterDynamicPropertyOfComponent(id, StringKey(propertyName));
}

void RegisterScriptEntityHandle(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("EntityHandle", sizeof(EntityHandle), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); OC_SCRIPT_ASSERT();

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

	// Register enum ePropertyAccess
	r = engine->RegisterEnum("ePropertyAccess"); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_EDIT_READ", Reflection::PA_EDIT_READ); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_EDIT_WRITE", Reflection::PA_EDIT_WRITE); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_SCRIPT_READ", Reflection::PA_SCRIPT_READ); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_SCRIPT_WRITE", Reflection::PA_SCRIPT_WRITE); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_INIT", Reflection::PA_INIT); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("ePropertyAccess", "PA_FULL_ACCESS", Reflection::PA_FULL_ACCESS); OC_SCRIPT_ASSERT();

	// Register typedef EntityID and PropertyAccessFlags
	r = engine->RegisterTypedef("EntityID", "int32");
	r = engine->RegisterTypedef("EntityTag", "uint16");
	r = engine->RegisterTypedef("PropertyAccessFlags", "uint8");

	// Register the object methods
	r = engine->RegisterObjectMethod("EntityHandle", "bool IsValid() const", asMETHOD(EntityHandle, IsValid), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "bool Exists() const", asMETHOD(EntityHandle, Exists), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "EntityID GetID() const", asMETHOD(EntityHandle, GetID), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "string GetName() const", asMETHOD(EntityHandle, GetName), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "EntityTag GetTag() const", asMETHOD(EntityHandle, GetTag), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "void SetTag(EntityTag)", asMETHOD(EntityHandle, SetTag), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "eEntityMessageResult PostMessage(const eEntityMessageType) const",
		asMETHODPR(EntityHandle, PostMessage, (const EntityMessage::eType) const, EntityMessage::eResult), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "eEntityMessageResult PostMessage(const eEntityMessageType, PropertyFunctionParameters) const",
		asMETHODPR(EntityHandle, PostMessage, (const EntityMessage::eType, Reflection::PropertyFunctionParameters) const, EntityMessage::eResult), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityHandle", "bool UnregisterDynamicProperty(const string &in) const",
		asFUNCTION(UnregisterDynamicProperty), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

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

static void Vector2Destructor(Vector2* self)
{
	OC_UNUSED(self);
	//
}

static Vector2& Vector2AssignOperator(const Vector2& other, Vector2* self)
{
	self->x = other.x;
	self->y = other.y;
	return *self;
}

inline static float32 Vector2GetX(const Vector2& self)
{
	return self.x;
}

inline static void Vector2SetX(Vector2& self, float32 value)
{
	self.x = value;
}

inline static float32 Vector2GetY(const Vector2& self)
{
	return self.y;
}

inline static void Vector2SetY(Vector2& self, float32 value)
{
	self.y = value;
}

void RegisterScriptVector2(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("Vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_APP_CLASS_C); OC_SCRIPT_ASSERT();

	// Register the object properties getters and setters
	//r = engine->RegisterObjectProperty("Vector2", "float32 x", offsetof(Vector2, x)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float32 get_x() const", asFUNCTION(Vector2GetX), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void set_x(float32)", asFUNCTION(Vector2SetX), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("Vector2", "float32 y", offsetof(Vector2, y)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float32 get_y() const", asFUNCTION(Vector2GetY), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void set_y(float32)", asFUNCTION(Vector2SetY), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(Vector2DefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(const Vector2 &in)", asFUNCTION(Vector2CopyConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(float32, float32)",  asFUNCTION(Vector2InitConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_DESTRUCT,   "void f()",  asFUNCTION(Vector2Destructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the operator overloads
	r = engine->RegisterObjectMethod("Vector2", "Vector2& opAssign(const Vector2 &in)", asFUNCTION(Vector2AssignOperator), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAddAssign(const Vector2 &in)", asMETHODPR(Vector2, operator+=, (const Vector2 &), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opSubAssign(const Vector2 &in)", asMETHODPR(Vector2, operator-=, (const Vector2 &), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opMulAssign(float32)", asMETHODPR(Vector2, operator*=, (float32), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void opAddAssign(const Vector2 &in)", asMETHODPR(Vector2, operator+=, (const Vector2 &), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void opSubAssign(const Vector2 &in)", asMETHODPR(Vector2, operator-=, (const Vector2 &), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void opMulAssign(float32)", asMETHODPR(Vector2, operator*=, (float32), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 &opDivAssign(float32)", asMETHODPR(Vector2, operator/=, (float32), Vector2&), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opNeg() const", asMETHOD(Vector2, operator-), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "bool opEquals(const Vector2 &in) const", asFUNCTIONPR(operator==, (const Vector2&, const Vector2&), bool), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const", asFUNCTIONPR(operator+, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const", asFUNCTIONPR(operator-, (const Vector2&, const Vector2&), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(float32) const", asFUNCTIONPR(operator*, (const Vector2&, float32), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul_r(float32) const", asFUNCTIONPR(operator*, (float32, const Vector2&), Vector2), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectMethod("Vector2", "Vector2 opDiv(float32) const", asFUNCTIONPR(operator/, (const Vector2&, float32), Vector2), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("Vector2", "float32 Length() const", asMETHOD(Vector2, Length), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float32 LengthSquared() const", asMETHOD(Vector2, LengthSquared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void Set(float32, float32)", asMETHODPR(Vector2, Set, (float32, float32), void), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void SetZero()", asMETHOD(Vector2, SetZero), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float32 Normalize()", asMETHOD(Vector2, Normalize), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "bool IsValid() const", asMETHOD(Vector2, IsValid), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float32 Dot(const Vector2 &in) const", asFUNCTION(MathUtils::Dot), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
}

// Functions for register EntityPicker to script

static void EntityPickerDefaultConstructor(EntityPicker* self)
{
	asSMessageInfo errorMessage;
	errorMessage.col = 0;
	errorMessage.row = 0;
	errorMessage.section = "Engine";
	errorMessage.type = (asEMsgType)0;
	errorMessage.message = "EntityPicker doesn't support default constructor";
	MessageCallback(&errorMessage, 0);
	new(self) EntityPicker(Vector2_Zero);
}

static void EntityPickerConstructor(const Vector2& worldCursorPos, const int32 minLayer, const int32 maxLayer, EntityPicker* self)
{
	new(self) EntityPicker(worldCursorPos, minLayer, maxLayer);
}

static void EntityPickerDestructor(EntityPicker* self)
{
	self->~EntityPicker();
}

void EntityPickerPickMultipleEntities(EntitySystem::EntityPicker& handle, asIScriptArray* entities, const Vector2& worldCursorPos, const float32 rotation)
{
	vector<EntitySystem::EntityHandle> pickedEntities;
	handle.PickMultipleEntities(worldCursorPos, rotation, pickedEntities);
	OC_ASSERT(entities);
	entities->Resize(pickedEntities.size());
	for (uint32 i = 0; i < pickedEntities.size(); ++i)
	{
		*(EntitySystem::EntityHandle*)entities->GetElementPointer(i) = pickedEntities[i];
	}
}

void RegisterScriptEntityPicker(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("EntityPicker", sizeof(EntityPicker), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("EntityPicker", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(EntityPickerDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("EntityPicker", asBEHAVE_CONSTRUCT, "void f(const Vector2 &in, const int32, const int32)", asFUNCTION(EntityPickerConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("EntityPicker", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(EntityPickerDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("EntityPicker", "EntityHandle PickSingleEntity()", asMETHOD(EntityPicker, PickSingleEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityPicker", "void PickMultipleEntities(EntityHandle[] &out, const Vector2 &in, const float32)", asFUNCTIONPR(EntityPickerPickMultipleEntities, (EntitySystem::EntityPicker&, asIScriptArray*, const Vector2&, const float32 rotation), void), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
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
	r = engine->RegisterObjectType("EntityDescription", sizeof(EntityDescription), asOBJ_VALUE | asOBJ_APP_CLASS_CD); OC_SCRIPT_ASSERT();

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

	// Register typedef ComponentID
	r = engine->RegisterTypedef("ComponentID", "int32");

	// Register the object methods
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle CreateEntity(EntityDescription &in)", asMETHOD(EntityMgr, CreateEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle InstantiatePrototype(const EntityHandle, const Vector2 &in, const string &in)", asMETHOD(EntityMgr, InstantiatePrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle DuplicateEntity(const EntityHandle, const string &in)", asMETHOD(EntityMgr, DuplicateEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void DestroyEntity(const EntityHandle)", asMETHOD(EntityMgr, DestroyEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool EntityExists(const EntityHandle) const", asMETHOD(EntityMgr, EntityExists), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle FindFirstEntity(const string &in)", asMETHOD(EntityMgr, FindFirstEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle FindFirstPrototype(const string &in)", asMETHOD(EntityMgr, FindFirstPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "EntityHandle GetEntity(EntityID) const", asMETHOD(EntityMgr, GetEntity), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool IsEntityInited(const EntityHandle) const", asMETHOD(EntityMgr, IsEntityInited), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool IsEntityPrototype(const EntityHandle) const", asMETHOD(EntityMgr, IsEntityPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void LinkEntityToPrototype(const EntityHandle, const EntityHandle)", asMETHOD(EntityMgr, LinkEntityToPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void UnlinkEntityFromPrototype(const EntityHandle)", asMETHOD(EntityMgr, UnlinkEntityFromPrototype), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "bool IsPrototypePropertyShared(const EntityHandle, const StringKey) const", asMETHOD(EntityMgr, IsPrototypePropertyShared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void SetPrototypePropertyShared(const EntityHandle, const StringKey)", asMETHOD(EntityMgr, SetPrototypePropertyShared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("EntityMgr", "void SetPrototypePropertyNonShared(const EntityHandle, const StringKey)", asMETHOD(EntityMgr, SetPrototypePropertyNonShared), asCALL_THISCALL); OC_SCRIPT_ASSERT();
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
	r = engine->RegisterGlobalFunction("EntityMgr& get_gEntityMgr()", asFUNCTION(GetEntityMgr), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

// Functions for register InputMgr to script

InputMgr& GetInputMgr()
{
	return gInputMgr;
}

inline static int32 MouseStateGetX(const MouseState& self)
{
	return self.x;
}

inline static void MouseStateSetX(MouseState& self, int32 value)
{
	self.x = value;
}

inline static int32 MouseStateGetY(const MouseState& self)
{
	return self.y;
}

inline static void MouseStateSetY(MouseState& self, int32 value)
{
	self.y = value;
}

inline static int32 MouseStateGetWheel(const MouseState& self)
{
	return self.wheel;
}

inline static void MouseStateSetWheel(MouseState& self, int32 value)
{
	self.wheel = value;
}

inline static uint8 MouseStateGetButtons(const MouseState& self)
{
	return self.buttons;
}

inline static void MouseStateSetButtons(MouseState& self, uint8 value)
{
	self.buttons = value;
}

void RegisterScriptInputMgr(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("InputMgr", 0, asOBJ_REF | asOBJ_NOHANDLE); OC_SCRIPT_ASSERT();

	// Register enum eKeyCode
	r = engine->RegisterEnum("eKeyCode"); OC_SCRIPT_ASSERT();
	for (uint8 code = 0; code < InputSystem::NUM_KEY_CODE; ++code)
	{
		const char* keyString = GetKeyCodeString(code);
		if (keyString != 0)
		{
			r = engine->RegisterEnumValue("eKeyCode", keyString, code); OC_SCRIPT_ASSERT();
		}
	}

	// Register struct MouseState
	r = engine->RegisterObjectType("MouseState", sizeof(MouseState), asOBJ_VALUE | asOBJ_POD); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("MouseState", "int32 x", offsetof(MouseState, x)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "int32 get_x() const", asFUNCTION(MouseStateGetX), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "void set_x(int32)", asFUNCTION(MouseStateSetX), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("MouseState", "int32 y", offsetof(MouseState, y)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "int32 get_y() const", asFUNCTION(MouseStateGetY), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "void set_y(int32)", asFUNCTION(MouseStateSetY), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("MouseState", "int32 wheel", offsetof(MouseState, wheel)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "int32 get_wheel() const", asFUNCTION(MouseStateGetWheel), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "void set_wheel(int32)", asFUNCTION(MouseStateSetWheel), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("MouseState", "uint8 buttons", offsetof(MouseState, buttons)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "uint8 get_buttons() const", asFUNCTION(MouseStateGetButtons), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("MouseState", "void set_buttons(uint8)", asFUNCTION(MouseStateSetButtons), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();


	// Register enum eMouseButton
	r = engine->RegisterEnum("eMouseButton"); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eMouseButton", "MBTN_LEFT", InputSystem::MBTN_LEFT); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eMouseButton", "MBTN_RIGHT", InputSystem::MBTN_RIGHT); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eMouseButton", "MBTN_MIDDLE", InputSystem::MBTN_MIDDLE); OC_SCRIPT_ASSERT();
	r = engine->RegisterEnumValue("eMouseButton", "MBTN_UNKNOWN", InputSystem::MBTN_UNKNOWN); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("InputMgr", "void CaptureInput()", asMETHOD(InputMgr, CaptureInput), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("InputMgr", "bool IsKeyDown(const eKeyCode) const", asMETHOD(InputMgr, IsKeyDown), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("InputMgr", "bool IsMouseButtonPressed(const eMouseButton) const", asMETHOD(InputMgr, IsMouseButtonPressed), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("InputMgr", "MouseState& GetMouseState() const", asMETHOD(InputMgr, GetMouseState), asCALL_THISCALL); OC_SCRIPT_ASSERT();

	// Register function that returns it
	r = engine->RegisterGlobalFunction("InputMgr& get_gInputMgr()", asFUNCTION(GetInputMgr), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

// Functions for register Project to script

string ProjectGetSceneName(int32 index, const Project* self)
{
	SceneInfoList list;
	self->GetSceneList(list);
	if (index >= 0 && index < (int32)list.size()) return list[index].name;
	else return "";
}

Project& GetProject()
{
	OC_ASSERT(gApp.GetGameProject());
	return *gApp.GetGameProject();
}

void RegisterScriptProject(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("Project", 0, asOBJ_REF | asOBJ_NOHANDLE); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("Project", "bool OpenScene(const string &in)", asMETHOD(Project, RequestOpenScene), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Project", "bool OpenSceneAtIndex(int32)", asMETHOD(Project, RequestOpenSceneAtIndex), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Project", "uint32 GetSceneCount() const", asMETHOD(Project, GetSceneCount), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Project", "int32 GetSceneIndex(const string &in) const", asMETHOD(Project, GetSceneIndex), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Project", "string GetOpenedSceneName() const", asMETHOD(Project, GetOpenedSceneName), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Project", "string GetSceneName(int32) const", asFUNCTION(ProjectGetSceneName), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register function that returns it
	r = engine->RegisterGlobalFunction("Project& get_gProject()", asFUNCTION(GetProject), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

// Functions for register Game to script

Game& GetGame()
{
	return GlobalProperties::Get<Core::Game>("Game");
}

void QuitGame(Game* self)
{
	OC_UNUSED(self);
	if (gApp.IsEditMode())
	{
		gEditorMgr.RestartAction();
		gEditorMgr.SwitchActionTool(Editor::EditorMgr::AT_RESTART);
	}
	else
	{
		gApp.Shutdown();
	}
}

void SetFullscreen(const bool value, Game* self)
{
	OC_UNUSED(self);
	if (!gApp.IsEditMode())
	{
		gGfxWindow.SetFullscreen(value);
	}
}

void RegisterScriptGame(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("Game", 0, asOBJ_REF | asOBJ_NOHANDLE); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("Game", "void ClearDynamicProperties()", asMETHOD(Game, ClearDynamicProperties), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "bool HasDynamicProperty(const string &in) const", asMETHOD(Game, HasDynamicProperty), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "bool DeleteDynamicProperty(const string &in)", asMETHOD(Game, DeleteDynamicProperty), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "bool LoadFromFile(const string &in)", asMETHOD(Game, LoadFromFile), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "bool SaveToFile(const string &in) const", asMETHOD(Game, SaveToFile), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "void PauseAction()", asMETHOD(Game, PauseAction), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "void ResumeAction()", asMETHOD(Game, ResumeAction), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "void Quit()", asFUNCTION(QuitGame), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "uint64 GetTime()", asMETHOD(Game, GetTimeMillis), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Game", "void SetFullscreen(const bool)", asFUNCTION(SetFullscreen), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register function that returns it
	r = engine->RegisterGlobalFunction("Game& get_game()", asFUNCTION(GetGame), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

// Functions for register Window to script

// Reference casting behaviour
template<class A, class B>
B* WindowRefCast(A* a)
{
	// If the handle already is a null handle, then just return the null handle
	if (!a) return 0;

	// Check if casting is correct (real class of a is derived from class B)
	if (!a->testClassName(B::EventNamespace)) return 0;

	// Now try to dynamically cast the pointer to the wanted type
	B* b = static_cast<B*>(a);
	WindowAddRef(b);

	return b;
}

template<class T>
void WindowAddRef(T* self)
{
	OC_UNUSED(self);
	// Do nothing
}

template<class T>
void WindowRelease(T* self)
{
	OC_UNUSED(self);
	// Do nothing
}

bool WindowIsDisabled(const CEGUI::Window* self)
{
	return self->isDisabled();
}

bool WindowIsVisible(const CEGUI::Window* self)
{
	return self->isVisible();
}

CEGUI::String WindowGetProperty(const CEGUI::String& propertyName, const CEGUI::Window* self)
{
	CEGUI_TRY;
	{
		return self->getProperty(propertyName);
	}
	CEGUI_CATCH;
	return "";
}

void WindowSetProperty(const CEGUI::String& propertyName, const CEGUI::String& value, CEGUI::Window* self)
{
	CEGUI_TRY;
	{
		self->setProperty(propertyName, value);
	}
	CEGUI_CATCH;
}

template<class T>
void RegisterScriptWindowMembers(asIScriptEngine *engine, const char *type)
{
	int32 r;

	// Register the object behaviour
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_ADDREF, "void f()", asFUNCTIONPR(WindowAddRef, (T*), void), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_RELEASE, "void f()", asFUNCTIONPR(WindowRelease, (T*), void), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod(type, "const CEGUIString& GetName() const", asMETHOD(T, getName), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "const CEGUIString& GetType() const", asMETHOD(T, getType), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsDisabled() const", asFUNCTION(WindowIsDisabled), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsDisabled(bool) const", asMETHOD(T, isDisabled), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsVisible() const", asFUNCTION(WindowIsVisible), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsVisible(bool) const", asMETHOD(T, isVisible), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsActive() const", asMETHOD(T, isActive), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "const CEGUIString& GetText() const", asMETHOD(T, getText), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool InheritsAlpha() const", asMETHOD(T, inheritsAlpha), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "float32 GetAlpha() const", asMETHOD(T, getAlpha), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "float32 GetEffectiveAlpha() const", asMETHOD(T, getEffectiveAlpha), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "Window@ GetParent() const", asMETHOD(T, getParent), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "const CEGUIString& GetTooltipText() const", asMETHOD(T, getTooltipText), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool InheritsTooltipText() const", asMETHOD(T, inheritsTooltipText), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetEnabled(bool)", asMETHOD(T, setEnabled), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetVisible(bool)", asMETHOD(T, setVisible), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void Activate()", asMETHOD(T, activate), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void Deactivate()", asMETHOD(T, deactivate), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetText(const CEGUIString& in)", asMETHOD(T, setText), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetAlpha(float32)", asMETHOD(T, setAlpha), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetInheritsAlpha(bool)", asMETHOD(T, setInheritsAlpha), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetTooltipText(const CEGUIString& in)", asMETHOD(T, setTooltipText), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetInheritsTooltipText(bool)", asMETHOD(T, setInheritsTooltipText), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "CEGUIString GetProperty(const CEGUIString& in) const", asFUNCTION(WindowGetProperty), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetProperty(const CEGUIString& in, const CEGUIString& in)", asFUNCTION(WindowSetProperty), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

}

template<class T>
void RegisterScriptButtonBaseMembers(asIScriptEngine *engine, const char *type)
{
	RegisterScriptWindowMembers<T>(engine, type);

	int32 r;
	r = engine->RegisterObjectBehaviour("Window", asBEHAVE_REF_CAST, (string(type) + "@ f()").c_str(), asFUNCTION((WindowRefCast<CEGUI::Window, T>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_IMPLICIT_REF_CAST, "Window@ f()", asFUNCTION((WindowRefCast<T, CEGUI::Window>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	r = engine->RegisterObjectMethod(type, "bool IsHovering() const", asMETHOD(T, isHovering), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsPushed() const", asMETHOD(T, isPushed), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

template<class T>
void RegisterScriptCheckboxMembers(asIScriptEngine *engine, const char *type)
{
	RegisterScriptButtonBaseMembers<T>(engine, type);

	int32 r;
	r = engine->RegisterObjectBehaviour("ButtonBase", asBEHAVE_REF_CAST, (string(type) + "@ f()").c_str(), asFUNCTION((WindowRefCast<CEGUI::ButtonBase, T>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_IMPLICIT_REF_CAST, "ButtonBase@ f()", asFUNCTION((WindowRefCast<T, CEGUI::ButtonBase>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	r = engine->RegisterObjectMethod(type, "bool IsSelected() const", asMETHOD(T, isSelected), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetSelected(bool)", asMETHOD(T, setSelected), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

template<class T>
void RegisterScriptPushButtonMembers(asIScriptEngine *engine, const char *type)
{
	RegisterScriptButtonBaseMembers<T>(engine, type);

	int32 r;
	r = engine->RegisterObjectBehaviour("ButtonBase", asBEHAVE_REF_CAST, (string(type) + "@ f()").c_str(), asFUNCTION((WindowRefCast<CEGUI::ButtonBase, T>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_IMPLICIT_REF_CAST, "ButtonBase@ f()", asFUNCTION((WindowRefCast<T, CEGUI::ButtonBase>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
}

template<class T>
void RegisterScriptRadioButtonMembers(asIScriptEngine *engine, const char *type)
{
	RegisterScriptButtonBaseMembers<T>(engine, type);

	int32 r;
	r = engine->RegisterObjectBehaviour("ButtonBase", asBEHAVE_REF_CAST, (string(type) + "@ f()").c_str(), asFUNCTION((WindowRefCast<CEGUI::ButtonBase, T>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_IMPLICIT_REF_CAST, "ButtonBase@ f()", asFUNCTION((WindowRefCast<T, CEGUI::ButtonBase>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	r = engine->RegisterObjectMethod(type, "bool IsSelected() const", asMETHOD(T, isSelected), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetSelected(bool)", asMETHOD(T, setSelected), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "uint32 GetGroupID() const", asMETHOD(T, getGroupID), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetGroupID(uint32)", asMETHOD(T, setGroupID), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

template<class T>
void RegisterScriptEditboxMembers(asIScriptEngine *engine, const char *type)
{
	RegisterScriptWindowMembers<T>(engine, type);

	int32 r;
	r = engine->RegisterObjectBehaviour("Window", asBEHAVE_REF_CAST, (string(type) + "@ f()").c_str(), asFUNCTION((WindowRefCast<CEGUI::Window, T>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour(type, asBEHAVE_IMPLICIT_REF_CAST, "Window@ f()", asFUNCTION((WindowRefCast<T, CEGUI::Window>)), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	r = engine->RegisterObjectMethod(type, "bool HasInputFocus() const", asMETHOD(T, hasInputFocus), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsReadOnly() const", asMETHOD(T, isReadOnly), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsTextMasked() const", asMETHOD(T, isTextMasked), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "bool IsTextValid() const", asMETHOD(T, isTextValid), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "const CEGUIString& GetValidationString() const", asMETHOD(T, getValidationString), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "uint32 GetMaskCodePoint() const", asMETHOD(T, getMaskCodePoint), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "uint32 GetMaxTextLength() const", asMETHOD(T, getMaxTextLength), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetReadOnly(bool)", asMETHOD(T, setReadOnly), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetTextMasked(bool)", asMETHOD(T, setTextMasked), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetValidationString(const CEGUIString &in)", asMETHOD(T, setValidationString), asCALL_THISCALL); OC_SCRIPT_ASSERT();  
	r = engine->RegisterObjectMethod(type, "void SetMaskCodePoint(uint32)", asMETHOD(T, setMaskCodePoint), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod(type, "void SetMaxTextLength(uint32)", asMETHOD(T, setMaxTextLength), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

CEGUI::Window* ScriptGetWindow(string name)
{
	string fullName = USER_GUI_WINDOWS_PREFIX + name;
	return gGUIMgr.WindowExists(fullName) ? gGUIMgr.GetWindow(fullName) : 0;
}

void CEGUIStringConstructor(CEGUI::String* self)
{
	new(self) CEGUI::String();
}

void CEGUIStringDestructor(CEGUI::String* self)
{
	OC_UNUSED(self);
}

CEGUI::String StringToCEGUIString(const string* self)
{
	return CEGUI::String((const CEGUI::utf8*)self->c_str());
}

string CEGUIStringToString(const CEGUI::String* self)
{
	return string(self->c_str());
}

void RegisterScriptWindows(asIScriptEngine* engine)
{
	int32 r;

	// Register the CEGUI string as the class that can be easily cast from/to string
	r = engine->RegisterObjectType("CEGUIString", sizeof(CEGUI::String), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("CEGUIString", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(CEGUIStringConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("CEGUIString", asBEHAVE_IMPLICIT_VALUE_CAST, "string f() const", asFUNCTION(CEGUIStringToString), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_IMPLICIT_VALUE_CAST, "CEGUIString f() const", asFUNCTION(StringToCEGUIString), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("CEGUIString", asBEHAVE_DESTRUCT, "void f()",  asFUNCTION(CEGUIStringDestructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();

	// Register the Window class
	r = engine->RegisterObjectType("Window", 0, asOBJ_REF); OC_SCRIPT_ASSERT();
	RegisterScriptWindowMembers<CEGUI::Window>(engine, "Window");

	// Register the ButtonBase class
	r = engine->RegisterObjectType("ButtonBase", 0, asOBJ_REF); OC_SCRIPT_ASSERT();
	RegisterScriptButtonBaseMembers<CEGUI::ButtonBase>(engine, "ButtonBase");

	// Register the Checkbox class
	r = engine->RegisterObjectType("Checkbox", 0, asOBJ_REF); OC_SCRIPT_ASSERT();
	RegisterScriptCheckboxMembers<CEGUI::Checkbox>(engine, "Checkbox");

	// Register the PushButton class
	r = engine->RegisterObjectType("PushButton", 0, asOBJ_REF); OC_SCRIPT_ASSERT();
	RegisterScriptPushButtonMembers<CEGUI::PushButton>(engine, "PushButton");

	// Register the RadioButton class
	r = engine->RegisterObjectType("RadioButton", 0, asOBJ_REF); OC_SCRIPT_ASSERT();
	RegisterScriptRadioButtonMembers<CEGUI::RadioButton>(engine, "RadioButton");

	// Register the Editbox class
	r = engine->RegisterObjectType("Editbox", 0, asOBJ_REF); OC_SCRIPT_ASSERT();
	RegisterScriptEditboxMembers<CEGUI::Editbox>(engine, "Editbox");

	r = engine->RegisterGlobalFunction("Window@ GetWindow(string)", asFUNCTION(ScriptGetWindow), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

// Functions for register GUIMgr to script

GUIMgr& GetGUIMgr()
{
	return gGUIMgr;
}

void RegisterScriptGUIMgr(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("GUIMgr", 0, asOBJ_REF | asOBJ_NOHANDLE); OC_SCRIPT_ASSERT();

	// Register the object methods
	r = engine->RegisterObjectMethod("GUIMgr", "void LoadScheme(const string &in)", asMETHOD(GUIMgr, LoadProjectScheme), asCALL_THISCALL); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("GUIMgr", "Window@ LoadLayout(const CEGUIString &in, const CEGUIString &in)", asMETHOD(GUIMgr, LoadProjectLayout), asCALL_THISCALL); OC_SCRIPT_ASSERT();

	// Register function that returns it
	r = engine->RegisterGlobalFunction("GUIMgr& get_gGUIMgr()", asFUNCTION(GetGUIMgr), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

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

// Functions for register StringMgr to script

const string GetTextData(const StringKey& group, const StringKey& key)
{
	return string(gStringMgrProject.GetTextData(group, key).c_str());
}

const string GetTextData(const StringKey& key)
{
	return string(gStringMgrProject.GetTextData(key).c_str());
}

void RegisterScriptStringMgr(asIScriptEngine* engine)
{
	int32 r;

	// Register StringMgr methods as global functions
	r = engine->RegisterGlobalFunction("const string GetTextData(const StringKey &in, const StringKey &in)", asFUNCTIONPR(GetTextData, (const StringKey&, const StringKey&), const string), asCALL_CDECL); OC_SCRIPT_ASSERT();
	r = engine->RegisterGlobalFunction("const string GetTextData(const StringKey &in)", asFUNCTIONPR(GetTextData, (const StringKey&), const string), asCALL_CDECL); OC_SCRIPT_ASSERT();
}

void ScriptMgr::ConfigureEngine(void)
{
	int32 r;

	// The first character in the single quoted string will be interpreted as a single uint32 value instead of a string literal
	r = mEngine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, 1); OC_SCRIPT_ASSERT();

	// Register typedefs for real numbers
	r = mEngine->RegisterTypedef("float32", "float"); OC_SCRIPT_ASSERT();
	r = mEngine->RegisterTypedef("float64", "double"); OC_SCRIPT_ASSERT();

	// Register the script string type
	RegisterStdString(mEngine);

	// Register StringKey class and it's methods
	RegisterScriptStringKey(mEngine);

	// Register PropertyFunctionParameter class and it's methods
	RegisterScriptPropertyFunctionParameters(mEngine);

	// Register EntityHandle class and it's methods
	RegisterScriptEntityHandle(mEngine);

	// Register EntityDescription class and it's methods
	RegisterScriptEntityDescription(mEngine);

	// Register Vector2 class and it's methods
	RegisterScriptVector2(mEngine);

	// Register EntityPicker class and it's methods
	RegisterScriptEntityPicker(mEngine);

	// Register EntityMgr class and it's methods
	RegisterScriptEntityMgr(mEngine);

	// Register InputMgr class and it's methods
	RegisterScriptInputMgr(mEngine);

	// Register a function for getting current owner entity handle
	r = mEngine->RegisterGlobalFunction("EntityHandle get_this()",
		asFUNCTION(GetCurrentEntityHandle), asCALL_CDECL); OC_SCRIPT_ASSERT();

	// Register a call function on EntityHandle
	r = mEngine->RegisterObjectMethod("EntityHandle", "void CallFunction(string &in, PropertyFunctionParameters &in) const",
		asFUNCTION(EntityHandleCallFunction), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

	// Register StringMgr methods
	RegisterScriptStringMgr(mEngine);

	// Register Project methods
	RegisterScriptProject(mEngine);

	// Register Game methods
	RegisterScriptGame(mEngine);

	// Register CEGUI components
	RegisterScriptWindows(mEngine);

	// Register GUIMgr methods
	RegisterScriptGUIMgr(mEngine);

	// Register all additions in ScriptRegister.cpp to script
	RegisterAllAdditions(mEngine);

	// Register getters, setters and array for supported types of properties

#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
	/* Register println function */ \
	r = mEngine->RegisterGlobalFunction((string("void Println(const ") + typeName + " &in)").c_str(), asFUNCTIONPR(ScriptPrintln, (const typeClass&), void), asCALL_CDECL); OC_SCRIPT_ASSERT(); \
	/* Register convert to string operators */ \
	if (typeID != PT_STRING) \
	{\
	r = mEngine->RegisterObjectMethod("string", (string("string &opAssign(const ") + typeName + " &in)").c_str(), asFUNCTIONPR(AssignToString, (const typeClass&, string&), string&), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("string", (string("string &opAddAssign(const ") + typeName + " &in)").c_str(), asFUNCTIONPR(AddAssignToString, (const typeClass&, string&), string&), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("string", (string("string opAdd(const ") + typeName + " &in) const").c_str(), asFUNCTIONPR(AddStringType, (string&, const typeClass&), string), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("string", (string("string opAdd_r(const ") + typeName + " &in) const").c_str(), asFUNCTIONPR(AddTypeString, (const typeClass&, string&), string), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT(); \
	}\
	/* Register getter and setter */ \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string(typeName) + " Get_" + typeName + "(const string &in) const").c_str(), \
	asFUNCTIONPR(EntityHandleGetValue, (EntitySystem::EntityHandle&, const string&), typeClass), \
	asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("void Set_") + typeName + "(const string &in, const " + typeName + "&in) const").c_str(), \
	asFUNCTIONPR(EntityHandleSetValue, (EntitySystem::EntityHandle&, const string&, const typeClass&), void), \
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
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("array_") + typeName + " Get_array_" + typeName + "(string &in) const").c_str(), \
	asFUNCTIONPR(EntityHandleGetArrayValue, (EntitySystem::EntityHandle&, string&), ScriptArray<typeClass>), \
	asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("const array_") + typeName + " Get_const_array_" + typeName + "(string &in) const").c_str(), \
	asFUNCTIONPR(EntityHandleGetConstArrayValue, (EntitySystem::EntityHandle&, string&), const ScriptArray<typeClass>), \
	asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	/* Register method for registering dynamic properties */ \
	r = mEngine->RegisterObjectMethod("EntityHandle", (string("bool RegisterDynamicProperty_") + typeName + "(const string &in, const PropertyAccessFlags, const string &in) const").c_str(), \
	asFUNCTION(RegisterDynamicProperty<typeClass>), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	/* Register operator<< method for PropertyFunctionParameters */ \
	r = mEngine->RegisterObjectMethod("PropertyFunctionParameters", (string("PropertyFunctionParameters opShl(const ") + typeName + " &in) const").c_str(), \
	asFUNCTIONPR(PropertyFunctionParametersOperatorShl, (Reflection::PropertyFunctionParameters&, const typeClass&), Reflection::PropertyFunctionParameters), \
	asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("PropertyFunctionParameters", (string("PropertyFunctionParameters opShl(const array_") + typeName + " &in) const").c_str(), \
	asFUNCTIONPR(PropertyFunctionParametersOperatorShl, (Reflection::PropertyFunctionParameters&, const ScriptArray<typeClass>&), Reflection::PropertyFunctionParameters), \
	asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	/* Register getter and setter for the game dynamic properties */ \
	r = mEngine->RegisterObjectMethod("Game", (string(typeName) + " Get_" + typeName + "(const string &in) const").c_str(), \
	asMETHODPR(Game, GetDynamicProperty, (const string&) const, typeClass), asCALL_THISCALL); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("Game", (string("void Set_") + typeName + "(const string &in, const " + typeName + "&in)").c_str(), \
	asMETHODPR(Game, SetDynamicProperty, (const string&, const typeClass&), void), asCALL_THISCALL); OC_SCRIPT_ASSERT(); \
	/* Register array const and non-const getter */ \
	r = mEngine->RegisterObjectMethod("Game", (string("array_") + typeName + " Get_array_" + typeName + "(string &in) const").c_str(), \
	asFUNCTIONPR(GameGetArrayValue, (Core::Game&, string&), ScriptArray<typeClass>), \
	asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT(); \
	r = mEngine->RegisterObjectMethod("Game", (string("const array_") + typeName + " Get_const_array_" + typeName + "(string &in) const").c_str(), \
	asFUNCTIONPR(GameGetConstArrayValue, (Core::Game&, string&), const ScriptArray<typeClass>), \
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

	// we never execute whole context from the console
	mExecFromConsole = false;

	int32 funcId = ctx->GetCurrentFunction();
	const char* moduleName = GetFunctionModuleName(funcId);
	const char* funcDecl = GetFunctionDeclaration(funcId);
	OC_ASSERT(moduleName && funcDecl);
	int32 r;

	ocDebug << "Executing script function '" << funcDecl << "' in module '" << moduleName << "'.";

	// Set line callback function to avoid script cycling
	uint64 deadline = 0;
	if (timeOut != 0)
	{
		r = ctx->SetLineCallback(asFUNCTION(LineCallback), &deadline, asCALL_CDECL);
		OC_ASSERT_MSG(r >= 0, "Failed to register line callback function.");
		deadline = GetGlobalTime() + timeOut;
	}

	// Execute the script function and get result
	switch (ctx->Execute())
	{
	case asEXECUTION_ABORTED:  // Script was aborted due to time out.
		ocInfo << "Script: Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' was aborted due to time out.";
		return false;
	case asEXECUTION_SUSPENDED: // Script was suspended by itself. Caller can continue the script.
		ocDebug << "Script: Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' was suspended.";
		return true;
	case asEXECUTION_FINISHED: // Script was completed successfully
		ocDebug << "Script: Execution of script function '" << funcDecl << "' in module '" << moduleName
			<< "' completed successfully.";
		return true;
	case asEXECUTION_EXCEPTION: // Exception occured in the script
		ocInfo << "Script: Exception '" << ctx->GetExceptionString() << "' at line " <<
			ctx->GetExceptionLineNumber() << " in function '" << funcDecl <<
			"' in module '" << moduleName << "'!";
		return false;
	default:
		OC_NOT_REACHED();
		return false;
	}
}

int ExecuteStringEngine(asIScriptEngine *engine, const char *code, asIScriptModule *mod, asIScriptContext *ctx)
{
	// Wrap the code in a function so that it can be compiled and executed
	string funcCode = "void ExecuteString() {\n";
	funcCode += code;
	funcCode += "\n;}";

	// If no module was provided, get a dummy from the engine
	asIScriptModule *execMod = mod ? mod : engine->GetModule("ExecuteString", asGM_ALWAYS_CREATE);

	// Compile the function that can be executed
	asIScriptFunction *func = 0;
	int r = execMod->CompileFunction("ExecuteString", funcCode.c_str(), -1, 0, &func);
	if( r < 0 )
		return r;

	// If no context was provided, request a new one from the engine
	asIScriptContext *execCtx = ctx ? ctx : engine->CreateContext();
	r = execCtx->Prepare(func->GetId());
	if( r < 0 )
	{
		func->Release();
		if( !ctx ) execCtx->Release();
		return r;
	}

	// Execute the function
	r = execCtx->Execute();

	// Clean up
	func->Release();
	if( !ctx ) execCtx->Release();

	return r;
}

bool ScriptMgr::ExecuteString(const char* script, const char* moduleName)
{
	// here we assume the single string is executed only from the GUI console
	mExecFromConsole = true;

	// Get the module by name
	asIScriptModule* mod = GetModule(moduleName);
	if (mod == 0)
	{
		ocInfo << "Script: Script module '" << moduleName << "' not found!";
		return false;
	}

	bool result;

	// Execute the script string and get result
	switch(ExecuteStringEngine(mEngine, script, mod, 0))
	{
	case asERROR: // failed to build
	case asINVALID_CONFIGURATION:
	case asBUILD_IN_PROGRESS:
	case asEXECUTION_ABORTED:
	case asEXECUTION_SUSPENDED:
	case asEXECUTION_EXCEPTION:
		result = false;
		break;
	case asEXECUTION_FINISHED:
		result = true;
		break;
	default:
		OC_NOT_REACHED();
		result = false;
	}

	mExecFromConsole = false;

	return result;
}

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
	r = IncludeCallback(fileName, "", mScriptBuilder, 0);
	if (r < 0) return 0;

	// Build module
	r = mScriptBuilder->BuildModule();
	if (r < 0)
	{
		ocInfo << "Script: Failed to build module '" << fileName << "'!";
		return 0;
	}

	ocInfo << "Loaded script module " << fileName;

	return mEngine->GetModule(fileName, asGM_ONLY_IF_EXISTS);
}

int32 ScriptMgr::GetFunctionID(const char* moduleName, const char* funcDecl)
{
	// Get module by name
	asIScriptModule* mod = GetModule(moduleName);
	if (mod == 0)
	{
		// ocInfo << "Script: Script module '" << moduleName << "' not found!";
		return -1;
	}

	// Get function ID from declaration
	return mod->GetFunctionIdByDecl(funcDecl);
}

const char* ScriptMgr::GetFunctionModuleName(int32 funcId)
{
	const asIScriptFunction *function = mEngine->GetFunctionDescriptorById(funcId);
	return function ? function->GetModuleName() : 0;
}

const char* ScriptMgr::GetFunctionDeclaration(int32 funcId)
{
	const asIScriptFunction *function = mEngine->GetFunctionDescriptorById(funcId);
	return function ? function->GetDeclaration() : 0;
}

void ScriptMgr::DefineWord( const char* word )
{
	mScriptBuilder->DefineWord(word);
}

void ScriptMgr::UnloadModule(const char* fileName)
{
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
#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
case typeID: \
	errorCode = ctx->SetArg##scriptSetter(parameterIndex, ARGVALUE_##scriptSetter(parameter.GetData<typeClass>())); \
	break;
#include "Utils/Properties/PropertyTypes.h"
#undef PROPERTY_TYPE

default:
	OC_ASSERT_MSG(false, "Unknown or unsupported property type");
	}

	if (errorCode == -12)
	{
		ocError << "Can't set script function argument: invalid size of the argument";
		return false;
	}

	if (errorCode != 0)
	{
		ocError << "Can't set script function argument: error code = " << errorCode;
		return false;
	}

	return true;
}

uint64 ScriptSystem::ScriptMgr::GetGameTime() const
{
	return GlobalProperties::Get<Core::Game>("Game").GetTimeMillis();
}

uint64 ScriptSystem::ScriptMgr::GetGlobalTime() const
{
	return gApp.GetCurrentTimeMillis();
}
