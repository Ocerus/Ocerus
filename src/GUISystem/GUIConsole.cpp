#include "Common.h"
#include "GUIConsole.h"

#include "CEGUICommon.h"
#include "ScriptSystem/ScriptMgr.h"

using namespace GUISystem;

GUIConsole::GUIConsole(): mIsInited(false), mCurrentLogLevelTreshold(0), mWidget(0), mPromptBoxWidget(0), mMessageBoxWidget(0)
{
	AppendEmptyHistoryCommand();
}

GUIConsole::~GUIConsole()
{
}

void GUIConsole::Init()
{
	OC_DASSERT(mWidget == 0);
	OC_DASSERT(mPromptBoxWidget == 0);
	OC_DASSERT(mMessageBoxWidget == 0);
	if (gGUIMgr.GetGUISheet() == 0)
	{
		ocError << "Cannot initialize GUIConsole. GUISheet was not set yet.";
		return;
	}
	CEGUI_TRY;
	{
		mWidget = gGUIMgr.LoadSystemLayout("GUIConsole.layout");
		gGUIMgr.GetGUISheet()->addChildWindow(mWidget);
		mPromptBoxWidget = gGUIMgr.GetWindow("GUIConsole/PromptBox");
		mMessageBoxWidget = (CEGUI::Listbox*)gGUIMgr.GetWindow("GUIConsole/MessageBox");
		mPromptBoxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&GUIConsole::OnEventKeyDown, this));
		mIsInited = true;
	}
	CEGUI_CATCH;
}

void GUIConsole::Deinit()
{
	if (!mIsInited)
		return;

	CEGUI_TRY;
	{
		gGUIMgr.DestroyWindow(mWidget);
		mWidget = mPromptBoxWidget = mMessageBoxWidget = 0;
	}
	CEGUI_CATCH;
}

void GUIConsole::AppendLogMessage(const string& logMessage, int32 logLevel)
{
	if (!mMessageBoxWidget) return;
	if (logLevel >= mCurrentLogLevelTreshold)
	{
		AppendMessage(logMessage);
	}
}

void GUIConsole::AppendScriptMessage(const string& message)
{
	AppendMessage(message);
}

void GUISystem::GUIConsole::AppendMessage( const string& message )
{
	CEGUI::Scrollbar* scrollbar = mMessageBoxWidget->getVertScrollbar();
	OC_ASSERT(scrollbar);
	bool scrolledDown = scrollbar->getScrollPosition() == scrollbar->getDocumentSize() - scrollbar->getPageSize();
	mMessageBoxWidget->addItem(new CEGUI::ListboxTextItem(message, 0, 0, true));
	if (scrolledDown)
	{
		scrollbar->setScrollPosition(scrollbar->getDocumentSize());
	}
	mMessageBoxWidget->getHorzScrollbar()->hide();
}

void GUIConsole::ToggleConsole()
{
	if (!mIsInited)
		return;

	if (!mWidget->isVisible())
	{
		// show
		// fix the scrollbar
		mMessageBoxWidget->getVertScrollbar()->setScrollPosition(mMessageBoxWidget->getVertScrollbar()->getDocumentSize());
		mWidget->setVisible(true);
		mWidget->setModalState(true);
		mPromptBoxWidget->activate();
	}
	else
	{
		// hide
		mWidget->setVisible(false);
		mWidget->setModalState(false);
	}
}

bool GUIConsole::OnEventKeyDown(const CEGUI::EventArgs& args)
{
	OC_DASSERT(mIsInited);
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	bool controlPressed = (keyArgs.sysKeys & CEGUI::Control) != 0;
	switch (keyArgs.scancode)
	{
		case CEGUI::Key::C:
			if (controlPressed)
			{
				ResetCommand();
				return true;
			}
			break;
		case CEGUI::Key::Return:
		case CEGUI::Key::NumpadEnter:
			SubmitCommand();
			return true;
		case CEGUI::Key::ArrowUp:
			HistoryUp();
			return true;
		case CEGUI::Key::ArrowDown:
			HistoryDown();
			return true;
		default:
			break;
	}
	return false;
}

void GUISystem::GUIConsole::SubmitCommand()
{
	string message(mPromptBoxWidget->getText().c_str());
	if (message.empty()) return;
	mPromptBoxWidget->setText("");

	mCurrentHistoryItem->modifiedCommand.clear();

	mHistory.back().originalCommand = message;
	mHistory.back().modifiedCommand.clear();

	AppendEmptyHistoryCommand();

	ScriptSystem::ScriptMgr::GetSingleton().ExecuteString(message.c_str());
}

void GUISystem::GUIConsole::ResetCommand()
{
	mPromptBoxWidget->setText("");
}

void GUISystem::GUIConsole::HistoryUp()
{
	if (mCurrentHistoryItem == mHistory.begin())
		return;

	SaveCurrentHistoryCommand();
	mCurrentHistoryItem--;
	LoadCurrentHistoryCommand();
}

void GUISystem::GUIConsole::HistoryDown()
{
	if (mCurrentHistoryItem + 1 == mHistory.end())
		return;

	SaveCurrentHistoryCommand();
	mCurrentHistoryItem++;
	LoadCurrentHistoryCommand();
}

void GUIConsole::SaveCurrentHistoryCommand()
{
	string currentCommand(mPromptBoxWidget->getText().c_str());
	if (!mCurrentHistoryItem->modifiedCommand.empty())
	{
		mCurrentHistoryItem->modifiedCommand = currentCommand;
	}
	else if (mCurrentHistoryItem->originalCommand != currentCommand)
	{
		mCurrentHistoryItem->modifiedCommand = currentCommand;
	}
}

void GUIConsole::LoadCurrentHistoryCommand()
{
	if (!mCurrentHistoryItem->modifiedCommand.empty())
	{
		mPromptBoxWidget->setText(mCurrentHistoryItem->modifiedCommand);
	}
	else
	{
		mPromptBoxWidget->setText(mCurrentHistoryItem->originalCommand);
	}
}

void GUIConsole::AppendEmptyHistoryCommand()
{
	mHistory.push_back(HistoryCommand(""));
	mCurrentHistoryItem = mHistory.end() - 1;
}

