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

		/// Function for testing
		void TestFunction(Reflection::PropertyFunctionParameters params)
		{
			const string* param0 = params.GetParameter(0).GetData<string>();
			const int32* param1 = params.GetParameter(1).GetData<int32>();
			if (param0 && param1) ocInfo << "The first parameter is '" << *param0 << "', the second is '" << *param1 << "'.";
			else ocInfo << "Wrong type of parameters!";
		}

		Utils::Array<int32>* GetArrayTest(void) const { return mArrayTest; }
	private:
		string mOnInitScript;
		uint32 mOnInitTimeOut;
		Utils::Array<int32>* mArrayTest;

		bool RunScript(void);
	};
}

#endif