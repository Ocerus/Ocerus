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
		static void TestRunTime();

		/// Names of the script modules that are searched for script message handlers.
		//@{
		Utils::Array<string>* GetModules(void) const { return const_cast<Utils::Array<string>*>(&mModules); }
		void SetModules(Utils::Array<string>* modules) { mModules.CopyFrom(*modules); }
		//@}
		/// Maximum time of execution the scripts in ms (0 means infinity).
		//@{
		uint32 GetTimeOut(void) const { return mTimeOut; }
		void SetTimeOut(const uint32 timeOut) { mTimeOut = timeOut; }
		//@}

		/// Find message handlers in modules (mModules) and store them to map (mMessageHandlers)
		void UpdateMessageHandlers(void);
	private:
		Utils::Array<string> mModules;
		uint32 mTimeOut;
		bool mNeedUpdate;

		/// Stores appropriate handler to message
		map<EntitySystem::EntityMessage::eType, int32> mMessageHandlers;

		void AnalyseModule(const string& module);
	};
}

#endif