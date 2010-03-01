#ifndef _GUIConsole_H_
#define _GUIConsole_H_

#include "Base.h"

namespace CEGUI
{
	class EventArgs;
	class Listbox;
	class Window;
}

namespace GUISystem
{
	class GUIMgr;

	/// The GUIConsole class manages the in-game console.
	class GUIConsole
	{
	friend class GUIMgr;
	public:
		/// Initializes the console. CEGUI must be already initialized before calling this method.
		void Init();

		/// Deinitializes the console.
		void Deinit();

		/// Appends log message to console, iff logLevel is greater or equal than current log level treshold.
		void AppendLogMessage(const string& logMessage, int32 logLevel);

		/// This method is used to write to console. It should be used by scripts, that were called from the console.
		void AppendScriptMessage(const string& message);

		/// Sets new log treshold.
		inline void SetLogLevelTreshold(int32 newLogLevelTreshold);

		/// Toggle console visibility. This is usually called after ` is pressed.
		void ToggleConsole();

	private:
		GUIConsole();
		~GUIConsole();

		/// This callback is called when user types a command to the console and presses enter.
		bool CommandHandler(const CEGUI::EventArgs& e);

		bool mIsInited;
		int32 mCurrentLogLevelTreshold;

		CEGUI::Window* mConsoleWidget;
		CEGUI::Window* mConsolePromptWidget;
		CEGUI::Listbox* mConsoleMessagesWidget;


	};

	inline void GUIConsole::SetLogLevelTreshold(int32 newLogLevelTreshold)
	{
		mCurrentLogLevelTreshold = newLogLevelTreshold;
	}
}


#endif // _GUIConsole_H_