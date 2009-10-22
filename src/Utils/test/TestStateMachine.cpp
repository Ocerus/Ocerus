#include "Common.h"
#include "UnitTests.h"
#include "../StateMachine.h"

class StateMachineMock: public StateMachine<int>
{
public:
	StateMachineMock(): StateMachine<int>(0), mIsChangeStateExpected(0),
		mExpectedOldState(-1), mExpectedNewState(-1)
	{
	}

	virtual void StateChanged(int oldState, int newState)
	{
		CHECK_EQUAL(mIsChangeStateExpected, true);
		CHECK_EQUAL(mExpectedOldState, oldState);
		CHECK_EQUAL(mExpectedNewState, newState);
		mIsChangeStateExpected = 0;
	}

	void setChangeStateExpected(int oldState, int newState)
	{
		mIsChangeStateExpected = true;
		mExpectedOldState = oldState;
		mExpectedNewState = newState;
	}

private:
	bool mIsChangeStateExpected;
	int mExpectedOldState;
	int mExpectedNewState;
};

SUITE(StateMachine)
{
	TEST(StateMachineIntTest)
	{
		Utils::StateMachine<int> stateMachine(0);
		CHECK_EQUAL(0, stateMachine.GetState());
		stateMachine.RequestStateChange(1, 1);
		CHECK_EQUAL(0, stateMachine.GetState());
		stateMachine.RequestStateChange(2, 1);
		CHECK_EQUAL(0, stateMachine.GetState());
		stateMachine.UpdateState();
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.UpdateState();
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.RequestStateChange(2, 0);
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.RequestStateChange(3, 0);
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.UpdateState();
		CHECK_EQUAL(3, stateMachine.GetState());
		stateMachine.RequestStateChange(4, 1);
		CHECK_EQUAL(3, stateMachine.GetState());
		stateMachine.UnlockState(0);
		stateMachine.RequestStateChange(5, 0);
		stateMachine.UpdateState();
		CHECK_EQUAL(4, stateMachine.GetState());
	}

	TEST(StateMachineCallbackTest)
	{
		StateMachineMock stateMachine;
		CHECK_EQUAL(0, stateMachine.GetState());
		stateMachine.RequestStateChange(1, 1);
		CHECK_EQUAL(0, stateMachine.GetState());
		stateMachine.RequestStateChange(2, 1);
		CHECK_EQUAL(0, stateMachine.GetState());
		stateMachine.setChangeStateExpected(0, 1);
		stateMachine.UpdateState();
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.UpdateState();
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.RequestStateChange(2, 0);
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.RequestStateChange(3, 0);
		CHECK_EQUAL(1, stateMachine.GetState());
		stateMachine.setChangeStateExpected(1, 3);
		stateMachine.UpdateState();
		CHECK_EQUAL(3, stateMachine.GetState());
		stateMachine.RequestStateChange(4, 1);
		CHECK_EQUAL(3, stateMachine.GetState());
		stateMachine.UnlockState(0);
		stateMachine.RequestStateChange(5, 0);
		stateMachine.setChangeStateExpected(3, 4);
		stateMachine.UpdateState();
		CHECK_EQUAL(4, stateMachine.GetState());

	}
}
