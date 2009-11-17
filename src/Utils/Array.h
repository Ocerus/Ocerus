/// @file
/// Templated representation of an array.

#ifndef Array_h__
#define Array_h__

#include "Base.h"

namespace Utils
{
	/// Templated representation of an array with the information about its size.
	/// The array knows its size and it can be resized. The purpose of this class is to allow access
	/// to an array in the same way as to C++ objects and to access arrays from scripts.
	/// There should be no performance decrease since every method is inlined.
	template<typename T>
	class Array
	{
	public:

		/// Constructs an empty array.
		inline Array(void): mSize(0), mData(0) {}

		/// Constructs new array with a given size.
		inline Array(const int32 size): mSize(size)
		{
			mData = new T[size];
		}

		/// Destructs the array correctly.
		~Array(void)
		{
			Clear();
		}

		/// Destroys the data the array carries.
		void Clear(void)
		{
			if (mData) delete[] mData;
			mData = 0;
			mSize = 0;
		}

		/// Read accessor to an array item.
		inline T operator[](const int32 index) const { return mData[index]; }

		/// Write accessor to an array item.
		inline T& operator[](const int32 index) { return mData[index]; }

		/// Returns size of the array.
		inline int32 GetSize(void) const { return mSize; }

		/// Returns the C-like array pointer.
		inline T* GetRawArrayPtr(void) const { return mData; }

		/// Resize array to newSize.
		void Resize(const int32 newSize)
		{
			OC_ASSERT(newSize>=0);
			if (mSize == newSize) return;

			T* newData = new T[newSize];
			// Copy old data to new array
			for (int32 i=0; i<newSize && i<mSize; ++i)
			{
				newData[i] = mData[i];
			}
			if (mData) delete[] mData;
			mData = newData;
			mSize = newSize;
		}

		/// Makes a deep copy of the array.
		void CopyFrom(const Array<T>& toCopy)
		{
			Resize(toCopy.GetSize());
			for (int32 i=0; i<mSize; ++i)
			{
				mData[i] = toCopy[i];
			}
		}

	private:
		T* mData;

		int32 mSize;
		/// Copy ctor and assignment operator are disabled.
		Array(const Array& rhs);
		Array& operator=(const Array& rhs);
	};
}


#endif // Array_h__