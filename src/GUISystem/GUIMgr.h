#ifndef _GUIMGR_H_
#define _GUIMGR_H_

#include "../Utility/Settings.h"
#include "../ResourceSystem/ResourceMgr.h"

#define gGUIMgr GUISystem::GUIMgr::GetSingleton()

//#pragma once

namespace GUISystem {

	typedef CEGUI::DirectX81Renderer DirectX81Renderer;
	typedef CEGUI::DirectX9Renderer DirectX9Renderer;
	typedef CEGUI::OpenGLRenderer OpenGLRenderer;
	typedef CEGUI::OgreCEGUIRenderer OgreCEGUIRenderer;
	typedef CEGUI::IrrlichtRenderer IrrlichtRenderer;
	typedef CEGUI::UDim UDim;
	typedef CEGUI::UVector2 UVector2;
	typedef CEGUI::URect URect;

	enum MouseButton
	{
	   LeftButton,
	   RightButton,
	   MiddleButton,
	   X1Button,
	   X2Button,
	   MouseButtonCount,
	   NoButton
	};

	class GUIMgr : public Singleton<GUIMgr>
	{
	public:	
		GUIMgr();

		inline bool SetResourceManager(ResourceMgr*);
		inline bool SetResourceGroupDirectory(const String& Groupname, const String& Directory);
		inline bool SetDefaultResourceGroup(const String& Module, const String& Group); 
		/** Modules: Imageset, Font, Scheme, WidgetLookManager, WindowsManager, ScriptModule **/

		inline bool SetDefaultFont(const String&);
		inline bool SetDefaultMouseCursor(const String& Look, const String& CursorName);
		inline bool SetDefaultTooltip(const String&);

		inline String GetDefaultFont();
		inline String GetDefaultMouseCursor();
		inline String GetDefaultTooltip();

		inline bool InjectMouseMove( float DeltaX, float DeltaY );
		inline bool InjectMousePosition( float XPos, float YPos );
		inline bool InjectMouseLeaves();
		inline bool InjectMouseButtonDown( MouseButton Button );
		inline bool InjectMouseButtonUp( MouseButton Button );
		inline bool InjectKeyDown( uint KeyCode );
		inline bool InjectKeyUp( uint KeyCode );
		inline bool InjectChar( utf32 CodePoint );
		inline bool InjectMouseWheelChange( float Delta );
		inline bool InjectTimePulse( float TimeElapsed );


		virtual ~GUIMgr();

	protected:
		ResourceMgr* mResourceMgr;

	};
}
#endif
