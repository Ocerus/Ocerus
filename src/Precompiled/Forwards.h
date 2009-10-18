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

namespace GfxSystem
{
	class GfxRenderer;
	class ParticleSystemMgr;
	class IScreenListener;
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
}

namespace InputSystem
{
	class InputMgr;
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
