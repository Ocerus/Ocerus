
#define ARRAY_CLASS(TYPE) \
/** This array corresponding to TYPE[] can be passed into entity properties. You should use this instead of
	the built-in AngelScript array (TYPE[]). The usage is similar. */ \
class array_##TYPE \
{ \
public: \
\
	/** Default constructor should not be used. Instead, instance should be retrieved from the EntityHandle
	property getter methods Get_array_##TYPE() and Get_const_array_##TYPE(). */ \
	array_##TYPE(); \
\
	/** Write accessor to an array item. */ \
	TYPE& operator[](int32 index); \
\
	/** Read accessor to an array item. */ \
	TYPE operator[](int32 index) const; \
\
	/** Returns the size of the array. */ \
	int32 GetSize() const; \
\
	/** Resizes the array to a new size keeping data if possible. */ \
	void Resize(int32 newSize); \
};


ARRAY_CLASS(bool);
ARRAY_CLASS(int8);
ARRAY_CLASS(int16);
ARRAY_CLASS(int32);
ARRAY_CLASS(int64);
ARRAY_CLASS(uint8);
ARRAY_CLASS(uint16);
ARRAY_CLASS(uint32);
ARRAY_CLASS(uint64);
ARRAY_CLASS(float32);
ARRAY_CLASS(Vector2);
ARRAY_CLASS(Point);
ARRAY_CLASS(string);
ARRAY_CLASS(StringKey);
ARRAY_CLASS(EntityHandle);
ARRAY_CLASS(Color);
ARRAY_CLASS(eKeyCode);
