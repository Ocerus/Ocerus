#ifndef Containers_h__
#define Containers_h__


#include <utility>
template<typename _Ty1, typename _Ty2>
struct Pair: public std::pair<_Ty1, _Ty2> {
	Pair(): std::pair<_Ty1, _Ty2>() {}
	Pair(const _Ty1& _Val1, const _Ty2& _Val2): std::pair<_Ty1, _Ty2>(_Val1, _Val2) {}
	template<class _Other1, class _Other2>
	Pair(const std::pair<_Other1, _Other2>& _Right): std::pair<_Ty1, _Ty2>(_Right) {}
};

#include <vector>
template<typename T>
class Vector: public std::vector<T> 
{
public:
	Vector(): std::vector<T>() {}
	explicit Vector(size_type _Count): std::vector<T>(_Count) {}
	Vector(const _Myt& _Right): std::vector<T>(_Right) {}
};

#include <list>
template<typename T>
class List: public std::list<T> 
{
public:
	List(): std::list<T>() {}
	explicit List(size_type _Count): std::list<T>(_Count) {}
	List(const _Myt& _Right): std::list<T>(_Right) {}
};

#include <queue>
template<typename T>
class Queue: public std::queue<T> 
{
public:
	Queue(): std::queue<T>() {}
	explicit Queue(const container_type& _Cont): std::queue<T>(_Cont) {}
};

#include <deque>
template<typename T>
class Deque: public std::deque<T> 
{
public:
	Deque(): std::deque<T>() {}
	explicit Deque(size_type _Count): std::deque<T>(_Count) {}
	Deque(const _Myt& _Right): std::deque<T>(_Right) {}
};

#include <map>
template<typename Key, typename Value>
class Map: public std::map<Key, Value>
{
public:
	Map(): std::map<Key, Value>() {}
};

#include <hash_map>
template<typename Key, typename Value>
class HashMap: public stdext::hash_map<Key, Value>
{
public:
	HashMap(): stdext::hash_map<Key, Value>() {}
};

#include <set>
template<typename T>
class Set: public std::set<T> 
{
public:
	Set(): std::set<T>() {}
};

#include <algorithm>
#include <utility>
namespace Containers
{
	template<class _InIt, class _Ty>
	inline _InIt find(_InIt _First, _InIt _Last, const _Ty& _Val) { return std::find(_First, _Last, _Val); }

	template<class _Ty1, class _Ty2>
	inline Pair<_Ty1, _Ty2> make_pair(_Ty1 _Val1, _Ty2 _Val2) { return std::make_pair<_Ty1, _Ty2>(_Val1, _Val2); }
}


#endif // Containers_h__