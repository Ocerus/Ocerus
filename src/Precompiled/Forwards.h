/// @file
/// Forward declarations of all major classes in the project.

#ifndef Forwards_h__
#define Forwards_h__

namespace Core
{
	class Config;
	class Game;
	class LoadingScreen;
}

namespace EntitySystem
{
	class EntityMgr;
	class EntityHandle;
	struct EntityMessage;
	class EntityDescription;
	class EntityPicker;
	class ComponentMgr;
	class Component;
}

namespace SceneSystem
{
}

namespace GfxSystem
{
	class GfxRenderer;
	class IGfxWindowListener;
	//class ParticleSystemMgr;
	struct Color;
}

namespace GUISystem 
{
	class GUIMgr;
}

namespace InputSystem
{
	class InputMgr;
}

namespace LogSystem
{
	class LogMgr;
}

namespace ResourceSystem
{
	class ResourceMgr;
	class IResourceLoadingListener;
	class XMLResource;
	class XMLNodeIterator;
}

namespace InputSystem
{
	class InputMgr;
}

namespace ScriptSystem
{
	class ScriptMgr;
}

namespace StringSystem
{
	class StringMgr;
}

namespace Utils
{
	class DataContainer;
	class StringKey;
	class Timer;
}

namespace Reflection 
{ 
	class AbstractProperty;
	class PropertyList; 
	class RTTIBaseClass;
	class PropertyHolder;
	class PropertyFunctionParameters;
	template<class T> class TypedProperty;
}


#endif // Forwards_h__
