/// @file
/// Platform specific setting of containers allowed in the project.
/// @remarks Currently all containers are implemented using the STL containers. The STL containers are not used directly
/// to enforce higher level of encapsulation and to enable easier changes in the future.

#ifndef Containers_h__
#define Containers_h__


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
template<typename T>
class list: public std::list<T> 
{
public:
    typedef typename std::list<T>::size_type size_type;
    inline list(): std::list<T>() {}
	explicit inline list(size_type _Count): std::list<T>(_Count) {}
	inline list(const std::list<T>& _Right): std::list<T>(_Right) {}
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
template<typename Key, typename Value>
class map: public std::map<Key, Value>
{
public:
	inline map(): std::map<Key, Value>() {}
};

#if defined(_MSC_VER) || defined(__BORLANDC__)

	#include <hash_map>
	template<typename Key, typename Value>
	class hash_map: public stdext::hash_map<Key, Value>
	{
	public:
		inline hash_map(): stdext::hash_map<Key, Value>() {}
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
template<typename T>
class set: public std::set<T> 
{
public:
	inline set(): std::set<T>() {}
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

#include "Tree.h"


#endif // Containers_h__
