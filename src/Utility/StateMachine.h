#ifndef StateMachine_h__
#define StateMachine_h__

template<typename T> class StateMachine
{
public:
	StateMachine(T initialState): mState(initialState), mNextState(initialState), mLocked(false) {}
	virtual ~StateMachine() {}

	inline T GetState() { return mState; }

	bool RequestStateChange(T newState, bool lock = false)
	{
		if (mLocked)
			return false;
		mNextState = newState;
		mLocked = lock;
		return true;
	}

	bool UnlockState(T lockState)
	{
		if (mNextState != lockState)
			return false;
		mLocked = false;
	}

	void UpdateState()
	{
		if (mState != mNextState)
		{
			T oldState = mState;
			mState = mNextState;
			mLocked = false;
			StateChanged(oldState, mState);
		}
	}
	
	// callbacks
	virtual void StateChanged(T oldState, T newState) {}

private:
	T mState, mNextState;
	bool mLocked;
};

#endif // StateMachine_h__
