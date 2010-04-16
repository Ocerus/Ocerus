#include "Common.h"
#include "ResourceWindow.h"

#include "ResourceSystem/ResourceMgr.h"

#include "GUISystem/CEGUITools.h"

using namespace Editor;

bool ResourceComparator(const ResourceSystem::ResourcePtr& r1, const ResourceSystem::ResourcePtr& r2)
{
	return (r1->GetFilePath().compare(r2->GetFilePath()) > 0);
}

Editor::ResourceWindow::ResourceWindow()
{
}

Editor::ResourceWindow::~ResourceWindow()
{
}


void Editor::ResourceWindow::Init()
{
	CEGUI_EXCEPTION_BEGIN
	CEGUI::Window* w = GUISystem::LoadWindowLayout("ResourceWindow.layout", "EditorRoot/ResourceWindow");
	gGUIMgr.GetRootLayout()->addChildWindow(w);
	CEGUI::ItemListbox* list = static_cast<CEGUI::ItemListbox*>(w->getChild(w->getName() + "/List"));
	OC_ASSERT(list != 0);
	for (size_t i = 0; i < list->getContentPane()->getChildCount(); ++i)
	{
		CEGUI::Window* aa = list->getContentPane()->getChildAtIdx(i);
		ocWarning << aa->getName();
		ocWarning << aa->getProperty("SelectionBrush");
		aa->setProperty("SelectionBrush", "set:TaharezLook image:TextSelectionBrush");
		CEGUI::ItemEntry* bb = static_cast<CEGUI::ItemEntry*>(aa);
		bb->setSelected(true);
	}

	vector<ResourceSystem::ResourcePtr> resources;
	gResourceMgr.GetResources(resources);
	Containers::sort(resources.begin(), resources.end(), ResourceComparator);

	uint32 i = 0;
	vector<string> dirStack;
	for (vector<ResourceSystem::ResourcePtr>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		string path = (*it)->GetFileDir();

		uint32 depth = 0;
		size_t searchFrom = 0;
		size_t match = 0;
		do
		{
			match = path.find('/', searchFrom);
			if (match == string::npos)
				match = path.size();
			string dirName = path.substr(searchFrom, match - searchFrom);
			if ((depth == dirStack.size()) || (dirName != dirStack[depth]))
			{
				if (depth == dirStack.size())
				{
					dirStack.push_back(dirName);
				}
				else
				{
					dirStack.resize(depth + 1);
					dirStack[depth] = dirName;
				}

				CEGUI::ItemEntry* dirItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", list->getName() + "/DirItem" + StringConverter::ToString(i++)));
				dirItem->setText(string(depth * 4, ' ') + dirName);
				list->addChildWindow(dirItem);
			}
			
			++depth;
			searchFrom = match+1;
		}
		while (match != path.size());

		CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", list->getName() + "/Resource" + StringConverter::ToString(i++)));
		newItem->setText(string(depth * 4, ' ') + (*it)->GetName());
		list->addChildWindow(newItem);
	}
	
	CEGUI_EXCEPTION_END
}
