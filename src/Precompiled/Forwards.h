/// @file
/// Forward declarations of all major classes in the project.

#ifndef Forwards_h__
#define Forwards_h__


namespace Core
{
	class Config;
	class Game;
	class LoadingScreen;
	class Project;
}

namespace Editor
{
	class CreateProjectDialog;
	class EditorGUI;
	class EditorMgr;
	class EditorMenu;
	class EntityWindow;
	class PopupMenu;
	class ResourceWindow;
	class PrototypeWindow;
	class HierarchyWindow;
	class AbstractValueEditor;
	class LayerWindow;
	class KeyShortcuts;
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
	struct Point;
	struct Color;
	class PhysicsDraw;
}

namespace GUISystem 
{
	class GUIMgr;
	class VerticalLayout;
	class ResourceProvider;
	class GUIConsole;
	class ViewportWindow;
	class ScriptProvider;
	class TabNavigation;
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
	class FormatText;
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
