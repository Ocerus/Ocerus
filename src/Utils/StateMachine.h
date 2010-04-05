/// @file
/// Implementation of a basic finite state machine.

#ifndef StateMachine_h__
#define StateMachine_h__

namespace Utils
{
	/// Implementation of a basic finite state machine.
	/// State type must be given by the template parameter T.
	template<typename T> class StateMachine
	{
	public:

		/// Constructs the state machine and sets the initial state.
		StateMachine(T initialState): mState(initialState), mNextState(initialState), mLocked(false) {}

		virtual ~StateMachine() {}

		/// Returns the current state of the machine.
		inline T GetState() { return mState; }

		/// Immediately changes the state.
		void ForceStateChange(T newState)
		{
			RequestStateChange(newState);
			UpdateState();
		}

		/// Requests a change of state of the machine.
		/// The state will be changed after the machine is updated.
		/// If lock is set to true all subsequent requests to state change will be discarded until the machine is either
		/// unlocked or updated.
		bool RequestStateChange(T newState, bool lock = false)
		{
			if (mLocked)
				return false;
			mNextState = newState;
			mLocked = lock;
			return true;
		}

		/// Unlocks the current state if it's locked. The given state must match the state which was locked.
		bool UnlockState(T lockState)
		{
			if (mNextState != lockState)
				return false;
			mLocked = false;
			return true;
		}

		/// Updates the machine and changes its state if needed.
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

		/// Callback to all derived classes. It's called whenever the state changes.
		virtual void StateChanged(T /* oldState */, T /* newState */) {}

	private:
		T mState, mNextState;
		bool mLocked;
	};
}

#endif // StateMachine_h__
