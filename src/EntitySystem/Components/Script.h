/// @file
/// Component for script calling

#ifndef Script_h__
#define Script_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// This component allows entities to run scripts. It may run only scripts supported by ScriptSystem. The scripts are
	/// executed as a reaction to received EntityMessage messages. For more informations about scripts see ScriptSystem.
	class Script : public RTTIGlue<Script, Component>
	{
	public:

		/// Called after the component is created.
		virtual void Create(void);

		/// Called before the component is destroyed.
		virtual void Destroy(void);

		/// Called when a new message arrives.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Called from RTTI when the component is allowed to set up its properties.
		static void RegisterReflection(void);
		
		/// Only for testing purpose
		static void TestRunTime();

		/// Names of the script modules that are searched for script message handlers.
		Utils::Array<string>* GetModules(void) const { return const_cast<Utils::Array<string>*>(&mModules); }

		/// Names of the script modules that are searched for script message handlers.
		void SetModules(Utils::Array<string>* modules) { mModules.CopyFrom(*modules); }

		/// Maximum time of execution the scripts in ms (0 means infinity).
		uint32 GetTimeOut(void) const { return mTimeOut; }

		/// Maximum time of execution the scripts in ms (0 means infinity).
		void SetTimeOut(const uint32 timeOut) { mTimeOut = timeOut; }

		/// States of OnAction handlers
		Utils::Array<int32>* GetStates(void) const { return const_cast<Utils::Array<int32>*>(&mStates); }

		/// States of OnAction handlers
		void SetStates(Utils::Array<int32>* states) { mStates.CopyFrom(*states); }

		/// Times of execution of OnAction handlers
		Utils::Array<uint64>* GetTimes(void) const { return const_cast<Utils::Array<uint64>*>(&mTimes); }

		/// Times of execution of OnAction handlers
		void SetTimes(Utils::Array<uint64>* times) { mTimes.CopyFrom(*times); }

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