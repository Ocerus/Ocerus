#include "Common.h"
#include "ScriptRegister.h"
#include <angelscript.h>

using namespace ScriptSystem;
using namespace AngelScript;

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
	r = engine->RegisterObjectType("Vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the object properties getters and setters
	//r = engine->RegisterObjectProperty("Vector2", "float x", offsetof(Vector2, x)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float get_x() const", asFUNCTION(Vector2GetX), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void set_x(float)", asFUNCTION(Vector2SetX), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("Vector2", "float y", offsetof(Vector2, y)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "float get_y() const", asFUNCTION(Vector2GetY), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Vector2", "void set_y(float)", asFUNCTION(Vector2SetY), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

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

inline static uint8 ColorGetR(const GfxSystem::Color& self)
{
	return self.r;
}

inline static void ColorSetR(GfxSystem::Color& self, int8 value)
{
	self.r = value;
}

inline static uint8 ColorGetG(const GfxSystem::Color& self)
{
	return self.g;
}

inline static void ColorSetG(GfxSystem::Color& self, int8 value)
{
	self.g = value;
}

inline static uint8 ColorGetB(const GfxSystem::Color& self)
{
	return self.b;
}

inline static void ColorSetB(GfxSystem::Color& self, int8 value)
{
	self.b = value;
}

inline static uint8 ColorGetA(const GfxSystem::Color& self)
{
	return self.a;
}

inline static void ColorSetA(GfxSystem::Color& self, int8 value)
{
	self.a = value;
}

void RegisterScriptColor(asIScriptEngine* engine)
{
	int32 r;
	// Register the type
	r = engine->RegisterObjectType("Color", sizeof(GfxSystem::Color), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); OC_SCRIPT_ASSERT();

	// Register the object properties getters and setters
	//r = engine->RegisterObjectProperty("Color", "uint8 r", offsetof(GfxSystem::Color, r)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "uint8 get_r() const", asFUNCTION(ColorGetR), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "void set_r(uint8)", asFUNCTION(ColorSetR), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("Color", "uint8 g", offsetof(GfxSystem::Color, g)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "uint8 get_g() const", asFUNCTION(ColorGetG), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "void set_g(uint8)", asFUNCTION(ColorSetG), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("Color", "uint8 b", offsetof(GfxSystem::Color, b)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "uint8 get_b() const", asFUNCTION(ColorGetB), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "void set_b(uint8)", asFUNCTION(ColorSetB), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	//r = engine->RegisterObjectProperty("Color", "uint8 a", offsetof(GfxSystem::Color, a)); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "uint8 get_a() const", asFUNCTION(ColorGetA), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectMethod("Color", "void set_a(uint8)", asFUNCTION(ColorSetA), asCALL_CDECL_OBJFIRST); OC_SCRIPT_ASSERT();

	// Register the constructors and destructor
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ColorDefaultConstructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint8, uint8, uint8, uint8)", asFUNCTION(ColorInit4Constructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint8, uint8, uint8)", asFUNCTION(ColorInit3Constructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	r = engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(uint32)", asFUNCTION(ColorInit1Constructor), asCALL_CDECL_OBJLAST); OC_SCRIPT_ASSERT();
	
	// Register the object methods
	r = engine->RegisterObjectMethod("Color", "uint32 GetARGB() const", asMETHOD(GfxSystem::Color, GetARGB), asCALL_THISCALL); OC_SCRIPT_ASSERT();
}

void ScriptSystem::RegisterAllAdditions(AngelScript::asIScriptEngine* engine)
{
	// Register Vector2 class and it's methods
	RegisterScriptVector2(engine);

	// Register Color struct and it's methods
	RegisterScriptColor(engine);
}