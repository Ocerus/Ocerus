#include "Common.h"

#include "GUIConsole.h"
#include "CEGUITools.h"
#include "ScriptSystem/ScriptMgr.h"

#include "CEGUI.h"

using namespace GUISystem;

GUIConsole::GUIConsole(): mIsInited(false), mCurrentLogLevelTreshold(0), mConsoleWidget(0), mConsolePromptWidget(0), mConsoleMessagesWidget(0)
{
}

GUIConsole::~GUIConsole()
{
}

void GUIConsole::Init()
{
	OC_DASSERT(mConsoleWidget == 0);
	OC_DASSERT(mConsolePromptWidget == 0);
	OC_DASSERT(mConsoleMessagesWidget == 0);
	CEGUI_EXCEPTION_BEGIN
	mConsoleWidget = LoadWindowLayout("Console.layout");
	gCEGUIWM.getWindow("root")->addChildWindow(mConsoleWidget);
	mConsolePromptWidget = gCEGUIWM.getWindow("ConsoleRoot/ConsolePrompt");
	mConsoleMessagesWidget = (CEGUI::Listbox*)gCEGUIWM.getWindow("ConsoleRoot/Pane");
	mConsolePromptWidget->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
									   CEGUI::Event::Subscriber(&GUIConsole::CommandHandler, this));
	mIsInited = true;
	CEGUI_EXCEPTION_END
}

void GUIConsole::Deinit()
{
	CEGUI_EXCEPTION_BEGIN
	gCEGUIWM.destroyWindow(mConsoleWidget);
	mConsoleWidget = mConsolePromptWidget = mConsoleMessagesWidget = 0;
	CEGUI_EXCEPTION_END
}

void GUIConsole::AppendLogMessage(const string& logMessage, int32 logLevel)
{
	OC_UNUSED(logMessage);
	if (logLevel >= mCurrentLogLevelTreshold)
	{

	}
}

void GUIConsole::AppendScriptMessage(const string& message)
{
	OC_UNUSED(message);
}

void GUIConsole::ToggleConsole()
{
	if (!mIsInited)
		return;

	mConsoleWidget->setVisible(!mConsoleWidget->isVisible());
	mConsolePromptWidget->activate();
}


bool GUIConsole::CommandHandler(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	OC_DASSERT(mIsInited);
	string message(mConsolePromptWidget->getText().c_str());
	mConsolePromptWidget->setText("");
	ScriptSystem::ScriptMgr::GetSingleton().ExecuteString(message.c_str());
	return true;
}
