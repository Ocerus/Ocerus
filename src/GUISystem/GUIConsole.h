/// @file
/// Manages the in-game console.
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
	public:
		/// Creates a GUIConsole instance.
		GUIConsole();

		/// Destroys the GUIConsole instance.
		~GUIConsole();
		
		/// Initializes the console. CEGUI must be already initialized before calling this method.
		void Init();

		/// Deinitializes the console.
		void Deinit();

		/// Appends log message to console, iff logLevel is greater or equal than current log level treshold.
		void AppendLogMessage(const string& logMessage, int32 logLevel);

		/// This method is used to write to console. It should be used by scripts, that were called from the console.
		void AppendScriptMessage(const string& message);

		/// This method is used to write to console.
		void AppendMessage(const string& message);

		/// Sets new log treshold.
		inline void SetLogLevelTreshold(int32 newLogLevelTreshold);

		/// Toggle console visibility. This is usually called after ` is pressed.
		void ToggleConsole();

	private:

		/// This struct holds data about an item in command history.
		struct HistoryCommand
		{
			/// Constructs new HistoryCommand with given string.
			HistoryCommand(const string& command): originalCommand(command) {}

			string originalCommand;	///< The original command, that was submitted.
			string modifiedCommand;	///< The modified version of this command before submitting, or null string.
		};
		typedef deque<HistoryCommand> HistoryCommands;

		/// This callback is called when user presses a key.
		bool OnEventKeyDown(const CEGUI::EventArgs&);

		/// Command submitting and command history related methods.
		//@{

			void SubmitCommand();
			void ResetCommand();
			void HistoryUp();
			void HistoryDown();
			void SaveCurrentHistoryCommand();
			void LoadCurrentHistoryCommand();
			void AppendEmptyHistoryCommand();
		//@}

		bool mIsInited;
		int32 mCurrentLogLevelTreshold;

		CEGUI::Window* mWidget;
		CEGUI::Window* mPromptBoxWidget;
		CEGUI::Listbox* mMessageBoxWidget;

		HistoryCommands mHistory;
		HistoryCommands::iterator mCurrentHistoryItem;
		
		bool mSuppressAppendingMessage;
	};

	inline void GUIConsole::SetLogLevelTreshold(int32 newLogLevelTreshold)
	{
		mCurrentLogLevelTreshold = newLogLevelTreshold;
	}
}


#endif // _GUIConsole_H_