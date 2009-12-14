/// @file
/// Platform specific setting of containers allowed in the project.
/// @remarks Currently all containers are implemented using the STL containers. The STL containers are not used directly
/// to enforce higher level of encapsulation and to enable easier changes in the future.

#ifndef Containers_h__
#define Containers_h__

#include "Memory/StlPoolAllocator.h"


#include <utility>
template<typename _Ty1, typename _Ty2>
struct pair: public std::pair<_Ty1, _Ty2> {
	inline pair(): std::pair<_Ty1, _Ty2>() {}
	inline pair(const _Ty1& _Val1, const _Ty2& _Val2): std::pair<_Ty1, _Ty2>(_Val1, _Val2) {}
	template<class _Other1, class _Other2> inline pair(const std::pair<_Other1, _Other2>& _Right): std::pair<_Ty1, _Ty2>(_Right) {}
};



#include <vector>
template<typename T>
class vector: public std::vector<T> 
{
public:
    typedef typename std::vector<T>::size_type size_type;
    inline vector(): std::vector<T>() {}
	explicit inline vector(size_type _Count): std::vector<T>(_Count) {}
	inline vector(const std::vector<T>& _Right): std::vector<T>(_Right) {}
};



#include <list>

template<typename _Ty>
struct pooled_list
{  
	// these definitions are taken from VS STL to exactly mimick what will be allocated by the container
	// note that if the structure will be too small, an assert will be raised during the construction
	struct _Node;
	friend struct _Node;
	typedef _Node *_Nodeptr;	// _Node allocator must have ordinary pointers
	struct _Node
	{	
		_Nodeptr _Next;	// successor node, or first element if head
		_Nodeptr _Prev;	// predecessor node, or last element if head
		_Ty _Myval;	// the stored value, unused if head
		#ifdef __UNIX__
		int _Another;
		#endif
	};

	typedef typename pooled_list::_Node allocable;
	typedef std::list< _Ty, StlPoolAllocator<_Ty, allocable> > type; 
};

template<typename T>
class list: public pooled_list<T>::type
{
public:
	typedef typename pooled_list<T>::type parentType;
	typedef typename parentType::size_type size_type;
    inline list(): parentType() {}
	explicit inline list(size_type _Count): parentType(_Count) {}
	inline list(const parentType& _Right): parentType(_Right) {}
	inline list(const list& _Right): parentType(_Right) {}
};



#include <queue>
template<typename T>
class queue: public std::queue<T> 
{
public:
    typedef typename std::queue<T>::container_type container_type;
	inline queue(): std::queue<T>() {}
	explicit inline queue(const container_type& _Cont): std::queue<T>(_Cont) {}
};



#include <deque>
template<typename T>
class deque: public std::deque<T> 
{
public:
    typedef typename std::deque<T>::size_type size_type;
	inline deque(): std::deque<T>() {}
	explicit inline deque(size_type _Count): std::deque<T>(_Count) {}
	inline deque(const std::deque<T>& _Right): std::deque<T>(_Right) {}
};



#include <map>

template<typename _Key, typename _Ty>
struct pooled_map 
{ 
	// these definitions are taken from VS STL to exactly mimick what will be allocated by the container
	// note that if the structure will be too small, an assert will be raised during the construction
	typedef typename std::map<_Key, _Ty>::value_type value_type;
	struct _Node;
	friend struct _Node;
	typedef _Node *_Nodeptr;	// _Node allocator must have ordinary pointers
	struct _Node // tree node
	{	
		_Nodeptr _Left;	// left subtree, or smallest element if head
		_Nodeptr _Parent;	// parent, or root of tree if head
		_Nodeptr _Right;	// right subtree, or largest element if head
		value_type _Myval;	// the stored value, unused if head
		char _Color;	// _Red or _Black, _Black if head
		char _Isnil;	// true only if head (also nil) node
	};

	typedef typename pooled_map::_Node allocable;
	typedef std::map< _Key, _Ty, std::less<_Key>, StlPoolAllocator<value_type, allocable> > type; 
};

template<typename Key, typename Value>
class map: public pooled_map<Key, Value>::type
{
public:
	typedef typename pooled_map<Key, Value>::type parentType;
	inline map(): parentType() {}
};



template<typename _Key, typename _Ty>
struct pooled_multimap: public pooled_map<_Key, _Ty>
{
	typedef std::multimap< _Key, _Ty, std::less<_Key>, Memory::StlPoolAllocator<typename pooled_map<_Key, _Ty>::value_type, typename pooled_map<_Key, _Ty>::allocable> > type; 
};

template<typename Key, typename Value>
class multimap: public pooled_multimap<Key, Value>::type
{
public:
	typedef typename pooled_multimap<Key, Value>::type parentType;
	inline multimap(): parentType() {}
};



#if defined(_MSC_VER) || defined(__BORLANDC__)

	#include <hash_map>

	template<typename _Key, typename _Ty>
	struct pooled_hash_map 
	{ 
		typedef _Ty value_type;
		typedef typename pooled_list< std::pair<_Key, _Ty> >::allocable allocable; // hash_map stores its values in lists
		typedef stdext::hash_map< _Key, _Ty, stdext::hash_compare<_Key, std::less<_Key> >, StlPoolAllocator<value_type, allocable> > type; 
	};

	// Note that we are explicitely not using pooled_hashmap here because it uses the allocator for a vector internally,
	// which would break things up (can't use memory pools for vector efficiently).
	template<typename Key, typename Value>
	class hash_map: public stdext::hash_map<Key, Value>
	{
	public:
		typedef typename stdext::hash_map<Key, Value> parentType;
		inline hash_map(): parentType() {}
	};

#else

	template<typename Key, typename Value>
	class hash_map: public std::map<Key, Value>
	{
	public:
		inline hash_map(): std::map<Key, Value>() {}
	};

#endif



#include <set>

template<typename _Key>
struct pooled_set 
{ 
	// these definitions are taken from VS STL to exactly mimick what will be allocated by the container
	// note that if the structure will be too small, an assert will be raised during the construction
	typedef typename std::set<_Key>::value_type value_type;
	struct _Node;
	friend struct _Node;
	typedef _Node *_Nodeptr;	// _Node allocator must have ordinary pointers
	struct _Node // tree node
	{	
		_Nodeptr _Left;	// left subtree, or smallest element if head
		_Nodeptr _Parent;	// parent, or root of tree if head
		_Nodeptr _Right;	// right subtree, or largest element if head
		value_type _Myval;	// the stored value, unused if head
		char _Color;	// _Red or _Black, _Black if head
		char _Isnil;	// true only if head (also nil) node
		#ifdef __UNIX__
		int _Another1;
		#endif
	};

	typedef typename pooled_set::_Node allocable;
	typedef std::set< _Key, std::less<_Key>, Memory::StlPoolAllocator<value_type, allocable> > type; 
};

template<typename T>
class set: public pooled_set<T>::type
{
public:
	typedef typename pooled_set<T>::type parentType;
	inline set(): parentType() {}
};



#include <algorithm>
#include <utility>
namespace Containers
{
	template<class _InIt, class _Ty>
	inline _InIt find(_InIt _First, _InIt _Last, const _Ty& _Val) { return std::find(_First, _Last, _Val); }

	template<class _Ty1, class _Ty2>
	inline pair<_Ty1, _Ty2> make_pair(_Ty1 _Val1, _Ty2 _Val2) { return std::make_pair<_Ty1, _Ty2>(_Val1, _Val2); }
}



#include "../Utils/Tree.h"


#endif // Containers_h__
