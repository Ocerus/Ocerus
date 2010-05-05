/// @file
/// Forward declarations of all major classes in the project.

#ifndef Forwards_h__
#define Forwards_h__


namespace Core
{
	class Config;
	class Game;
	class PhysicsDraw;
	class LoadingScreen;
}

namespace Editor
{
	class EditorGUI;
	class EditorMgr;
	class EditorMenu;
	class PopupMenu;
	class ResourceWindow;
	class ProjectMgr;
	class PrototypeWindow;
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
	class GfxSceneMgr;
	class IGfxWindowListener;
	class DragDropCameraMover;
	struct Color;
}

namespace GUISystem 
{
	class GUIMgr;
	class VerticalLayout;
	class ResourceProvider;
	class GUIConsole;
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
	class XMLOutput;
}

namespace InputSystem
{
	class InputMgr;
}

namespace ScriptSystem
{
	class ScriptMgr;
	class ScriptResource;
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
