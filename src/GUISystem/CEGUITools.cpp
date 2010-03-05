#include "Common.h"

#include "GUIMgr.h"
#include "CEGUITools.h"

using namespace GUISystem;

/**
 * The PropertyCallback function is a callback used to translate textual data
 * from window layout. This callback should be used when loading window layouts
 * in CEGUI::WindowManager::loadWindowLayout.
 */
bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname, CEGUI::String& propvalue, void* userdata)
{
	OC_UNUSED(userdata);
	if (propname == "Text" &&
		propvalue.size() > 2 &&
		propvalue.at(0) == '$' &&
		propvalue.at(propvalue.size() - 1) == '$')
	{
		/// @todo Use StringMgr to translate textual data in GUI.
		CEGUI::String translatedText = "_" + propvalue.substr(1, propvalue.size() - 2);
		window->setProperty(propname, translatedText);
		return false;
	}
	return true;
}

CEGUI::Window* GUISystem::LoadWindowLayout(const CEGUI::String& filename, const CEGUI::String& name_prefix, const CEGUI::String& resourceGroup)
{
	return CEGUI::WindowManager::getSingleton().loadWindowLayout(filename, name_prefix, resourceGroup, PropertyCallback);
}