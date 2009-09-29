#ifndef StateMachine_h__
#define StateMachine_h__

/** Implementation of a basic finite state machine.
*/
template<typename T> class StateMachine
{
public:
	StateMachine(T initialState): mState(initialState), mNextState(initialState), mLocked(false) {}
	virtual ~StateMachine() {}

	/// @name Returns current state of the machine.
	inline T GetState() { return mState; }

	/** Requests a change of state of the machine. The state will be changed after the machine is updated.
		If lock is set to true all subsequent requests to state change will be discarded until the machine is either
		unlocked or updated.
	*/
	bool RequestStateChange(T newState, bool lock = false)
	{
		if (mLocked)
			return false;
		mNextState = newState;
		mLocked = lock;
		return true;
	}

	/// @name Unlocks current state if locked. The passed state must match the state which was locked.
	bool UnlockState(T lockState)
	{
		if (mNextState != lockState)
			return false;
		mLocked = false;
	}

	/// @name Updates the machine and changes its state if needed.
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
	
	/// @name Callbacks to all derived classes.
	virtual void StateChanged(T oldState, T newState) {}

private:
	T mState, mNextState;
	bool mLocked;
};

#endif // StateMachine_h__
