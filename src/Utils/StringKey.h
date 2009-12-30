/// @file
/// Search key represented by strings.

#ifndef StringKey_h__
#define StringKey_h__

#include "Base.h"

namespace Utils
{
	/// This class serves as a key into maps and other structures where we want to index data using strings, but we need high speed as well.
	/// The string value is hashed and the result is then used as a decimal representation of the string.
	class StringKey
	{
	public:

		/// Default constructor.
		StringKey(void);

		/// Constructs the key from a c-style string (null terminated).
		StringKey(const char* str);

		/// Constructs the key from a c-style string (null terminated).
		StringKey(const char* str, size_t length);

		/// Constructs the key from a standard string.
		StringKey(const string& str);

		/// Copy constructor.
		StringKey(const StringKey& rhs);

		~StringKey(void);

		/// Assignment operator.
		StringKey& operator=(const StringKey& rhs);

		/// Converts the key to a string.
		string ToString(void) const;

		/// Converts the key to a decimal value (hash).
        /// @note Is it really smart to have implicit conversion for this?
        ///       It introduces ambiguity when StringKeys are passed to our LogMgr (which should be rewritten anyway)
		operator size_t (void) const;

		/// Comparison operator needed by containers.
		inline bool operator==(const StringKey& rhs) const
		{
			return mData == rhs.mData;
		}

		/// Comparison operator needed by containers.
		bool operator<(const StringKey& rhs) const
		{
			return mData < rhs.mData;
		}

		/// Invalid key representing no real string.
		static const StringKey Null;

		struct StringKeyData
		{
			StringKeyData(string refString): mRefCount(0), mRefString(refString) {}
			uint8   mRefCount;
			string  mRefString;
		};

	private:
		void RefCntInc();
		void RefCntDec();

		StringKeyData* mData;
	};
}

#endif // StringKey_h__
