#include "Common.h"
#include "UnitTests.h"
#include "../COWPtr.h"

template<typename T>
class MockWithCounting
{
	T mData;
	static unsigned int smRefCount;

public:
	MockWithCounting() { ++smRefCount; }

	MockWithCounting(const T& data)
	{
		++smRefCount;
		mData = data;
	}

	MockWithCounting(const MockWithCounting<T>& o)
	{
		++smRefCount;
		mData = o.mData;
	}

	~MockWithCounting() { --smRefCount; }

	T GetData() const
	{
		return mData;
	}

	void SetData(const T& newData)
	{
		mData = newData;
	}

	static unsigned int RefCount()
	{
		return smRefCount;
	}
};

template<typename T> unsigned int MockWithCounting<T>::smRefCount(0);

SUITE(COWPtr)
{
	/// Tests whether the copy contract is held, i.e. the change of data referenced
	/// by copied pointer does not affect data referenced by original pointer.
	TEST(TestCopyContract)
	{
		COWPtr<int> myPtr1(new int(0));
		CHECK_EQUAL(*myPtr1, 0);
		COWPtr<int> myPtr2(myPtr1);
		CHECK_EQUAL(*myPtr2, 0);
		*myPtr2 = 42;
		CHECK_EQUAL(*myPtr1, 0);
		CHECK_EQUAL(*myPtr2, 42);
		*myPtr1 = -42;
		CHECK_EQUAL(*myPtr1, -42);
		CHECK_EQUAL(*myPtr2, 42);
		COWPtr<int> myPtr3(myPtr1);
		COWPtr<int> myPtr4(myPtr2);
		CHECK_EQUAL(*myPtr3, -42);
		CHECK_EQUAL(*myPtr4, 42);
		*myPtr1 = *myPtr4;
		*myPtr4 = 0;
		CHECK_EQUAL(*myPtr1, 42);
		CHECK_EQUAL(*myPtr2, 42);
		CHECK_EQUAL(*myPtr3, -42);
		CHECK_EQUAL(*myPtr4, 0);
	}

	/// Tests whether copies are made only when non-const operations/methods are used.
	TEST(TestLazyCopyContract)
	{
		CHECK_EQUAL(MockWithCounting<int>::RefCount(), 0U);
		COWPtr<MockWithCounting<int> > myPtr1(new MockWithCounting<int>(42));
		CHECK_EQUAL(MockWithCounting<int>::RefCount(), 1U);
		COWPtr<MockWithCounting<int> > myPtr2(myPtr1);
		CHECK_EQUAL(MockWithCounting<int>::RefCount(), 1U);
		COWPtr<MockWithCounting<int> > myPtr3(myPtr1);
		CHECK_EQUAL(MockWithCounting<int>::RefCount(), 1U);
		const COWPtr<MockWithCounting<int> >& rMyPtr1 = myPtr1;
		CHECK_EQUAL(rMyPtr1->GetData(), 42);
		CHECK_EQUAL(1U, MockWithCounting<int>::RefCount());
		CHECK_EQUAL(const_cast<const COWPtr<MockWithCounting<int> >& >(myPtr1)->GetData(), 42);
		CHECK_EQUAL(const_cast<const COWPtr<MockWithCounting<int> >& >(myPtr2)->GetData(), 42);
		CHECK_EQUAL(const_cast<const COWPtr<MockWithCounting<int> >& >(myPtr3)->GetData(), 42);
		CHECK_EQUAL(1U, MockWithCounting<int>::RefCount());
		myPtr1->SetData(100);
		CHECK_EQUAL(2U, MockWithCounting<int>::RefCount());
		myPtr1->SetData(1000);
		CHECK_EQUAL(2U, MockWithCounting<int>::RefCount());
		myPtr2->SetData(0);
		CHECK_EQUAL(3U, MockWithCounting<int>::RefCount());
		myPtr3->SetData(1);
		CHECK_EQUAL(3U, MockWithCounting<int>::RefCount());
	}
}

