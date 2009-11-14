/// @file
/// Templated representation of an array.

#ifndef Array_h__
#define Array_h__

#include "Base.h"

namespace Utils
{
	/// Templated representation of an array without the information about its size.
	/// The array doesn't know it's size. The purpose of this class is to allow access to an array in the
	/// same way as to C++ objects. There should be no performance decrease since every method is inlined.
	template<typename T>
	class Array
	{
	public:

		/// Constructs new array with a given size.
		inline Array(const int32 size): mSize(size)
		{
			mData = new T[size];
		}

		/// Destructs the array correctly.
		inline ~Array(void)
		{
			delete[] mData;
		}

		/// Read accessor to an array item.
		inline T operator[](const int32 index) const { return mData[index]; }

		/// Write accessor to an array item.
		inline T& operator[](const int32 index) { return mData[index]; }

		/// Returns size of the array
		inline int32 GetSize() const { return mSize; }

	private:
		T* mData;

		const int32 mSize;
		/// Copy ctor and assignment operator are disabled.
		Array(const Array& rhs);
		Array& operator=(const Array& rhs);
	};
}


#endif // Array_h__