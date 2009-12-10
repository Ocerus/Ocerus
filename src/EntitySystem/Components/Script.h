/// @file
/// Component for script calling

#ifndef Script_h__
#define Script_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Handles messages as script call
	class Script : public RTTIGlue<Script, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);
		
		/// Only for testing purpose
		static void TestRunTime();

		/// @name Names of the script modules that are searched for script message handlers.
		//@{
		Utils::Array<string>* GetModules(void) const { return const_cast<Utils::Array<string>*>(&mModules); }
		void SetModules(Utils::Array<string>* modules) { mModules.CopyFrom(*modules); }
		//@}

		/// @name Maximum time of execution the scripts in ms (0 means infinity).
		//@{
		uint32 GetTimeOut(void) const { return mTimeOut; }
		void SetTimeOut(const uint32 timeOut) { mTimeOut = timeOut; }
		//@}

		/// @name States of OnAction handlers
		//@{
		Utils::Array<int32>* GetStates(void) const { return const_cast<Utils::Array<int32>*>(&mStates); }
		void SetStates(Utils::Array<int32>* states) { mStates.CopyFrom(*states); }
		//@}

		/// @name Times of execution of OnAction handlers
		//@{
		Utils::Array<uint64>* GetTimes(void) const { return const_cast<Utils::Array<uint64>*>(&mTimes); }
		void SetTimes(Utils::Array<uint64>* times) { mTimes.CopyFrom(*times); }
		//@}

		/// Current index of mStates and mTimes
		int32 GetCurrentArrayIndex(void) const { return mCurrentArrayIndex; }
	private:
		Utils::Array<string> mModules;
		uint32 mTimeOut;
		Utils::Array<int32> mStates;
		Utils::Array<uint64> mTimes;
		int32 mCurrentArrayIndex;
		
		/// Whether the UpdateMessageHandlers will be executed when next message in handled.
		bool mNeedUpdate;

		/// Stores appropriate handler to message.
		multimap<EntitySystem::EntityMessage::eType, int32> mMessageHandlers;
		
		/// Map module name to function ID of OnAction handlers
		map<string, int32> mModuleToFuncID;
		
		/// Map function ID of OnAction handlers to index of mStates and mTimes
		map<int32, int32> mFuncIDToArrayIndex;		

		/// Find message handlers in modules (mModules) and store them to map (mMessageHandlers).
		void UpdateMessageHandlers(void);
	};
}

#endif