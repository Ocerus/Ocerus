/// @file
/// Script calling after inicialization of entity

#ifndef OnInitScript_h__
#define OnInitScript_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Handles script call after inicialization of entity.
	class OnInitScript : public RTTIGlue<OnInitScript, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);
		static void TestRunTime();

		/// Name of the script module 
		//@{
		string GetOnInitScript(void) const { return mOnInitScript; }
		void SetOnInitScript(const string script) { mOnInitScript = script; }
		//@}
		/// Maximum time of execution the script in ms (0 means infinity)
		//@{
		uint32 GetOnInitTimeOut(void) const { return mOnInitTimeOut; }
		void SetOnInitTimeOut(const uint32 timeOut) { mOnInitTimeOut = timeOut; }
		//@}
	private:
		string mOnInitScript;
		uint32 mOnInitTimeOut;

		bool RunScript(void);
	};
}

#endif