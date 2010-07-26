#include "Common.h"

#include "GUIConsole.h"
#include "CEGUITools.h"
#include "ScriptSystem/ScriptMgr.h"

#include "CEGUI.h"

using namespace GUISystem;

GUIConsole::GUIConsole(): mIsInited(false), mCurrentLogLevelTreshold(0), mConsoleWidget(0), mConsolePromptWidget(0), mConsoleMessagesWidget(0)
{
	AppendEmptyHistoryCommand();
}

GUIConsole::~GUIConsole()
{
}

void GUIConsole::Init()
{
	OC_DASSERT(mConsoleWidget == 0);
	OC_DASSERT(mConsolePromptWidget == 0);
	OC_DASSERT(mConsoleMessagesWidget == 0);
	OC_DASSERT(gGUIMgr.GetGUISheet() != 0);
	CEGUI_EXCEPTION_BEGIN
	mConsoleWidget = gGUIMgr.LoadSystemLayout("Console.layout");
	gGUIMgr.GetGUISheet()->addChildWindow(mConsoleWidget);
	mConsolePromptWidget = gCEGUIWM.getWindow("ConsoleRoot/ConsolePrompt");
	mConsoleMessagesWidget = (CEGUI::Listbox*)gCEGUIWM.getWindow("ConsoleRoot/Pane");
	mConsolePromptWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&GUIConsole::OnEventKeyDown, this));
	mIsInited = true;
	CEGUI_EXCEPTION_END
}

void GUIConsole::Deinit()
{
	if (!mIsInited)
		return;

	CEGUI_EXCEPTION_BEGIN
	gCEGUIWM.destroyWindow(mConsoleWidget);
	mConsoleWidget = mConsolePromptWidget = mConsoleMessagesWidget = 0;
	CEGUI_EXCEPTION_END
}

void GUIConsole::AppendLogMessage(const string& logMessage, int32 logLevel)
{
	if (!mConsoleMessagesWidget) return;
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
	CEGUI::Scrollbar* scrollbar = mConsoleMessagesWidget->getVertScrollbar();
	OC_ASSERT(scrollbar);
	bool scrolledDown = scrollbar->getScrollPosition() == scrollbar->getDocumentSize() - scrollbar->getPageSize();
	mConsoleMessagesWidget->addItem(new CEGUI::ListboxTextItem(message, 0, 0, true));
	if (scrolledDown)
	{
		scrollbar->setScrollPosition(scrollbar->getDocumentSize());
	}
	mConsoleMessagesWidget->getHorzScrollbar()->hide();
}

void GUIConsole::ToggleConsole()
{
	if (!mIsInited)
		return;

	if (!mConsoleWidget->isVisible())
	{
		// fix the scrollbar
		mConsoleMessagesWidget->getVertScrollbar()->setScrollPosition(mConsoleMessagesWidget->getVertScrollbar()->getDocumentSize());
	}

	mConsoleWidget->setVisible(!mConsoleWidget->isVisible());
	mConsolePromptWidget->activate();
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
	string message(mConsolePromptWidget->getText().c_str());
	if (message.empty()) return;
	mConsolePromptWidget->setText("");

	mCurrentHistoryItem->modifiedCommand.clear();

	mHistory.back().originalCommand = message;
	mHistory.back().modifiedCommand.clear();

	AppendEmptyHistoryCommand();

	ScriptSystem::ScriptMgr::GetSingleton().ExecuteString(message.c_str());
}

void GUISystem::GUIConsole::ResetCommand()
{
	mConsolePromptWidget->setText("");
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
	string currentCommand(mConsolePromptWidget->getText().c_str());
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
		mConsolePromptWidget->setText(mCurrentHistoryItem->modifiedCommand);
	}
	else
	{
		mConsolePromptWidget->setText(mCurrentHistoryItem->originalCommand);
	}
}

void GUIConsole::AppendEmptyHistoryCommand()
{
	mHistory.push_back(HistoryCommand(""));
	mCurrentHistoryItem = mHistory.end() - 1;
}

