#ifndef _XMLRESOURCE_H_
#define _XMLRESOURCE_H_

#include "Common.h"
#include "Resource.h"
#include "../Utility/Settings.h"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "expat.h"
#include <cctype>
#include <algorithm>
#include <fstream>

// Expat constatns
#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192

namespace ResourceSystem 
{
	class XMLResource; 

	/// @name define a container for textdata
	typedef std::map<string, string> XMLDataMap;
	/// @name define pointer to XMLResource
	typedef SmartPointer<XMLResource> XMLResourcePtr;

	/// @name Functions used by Expat parser
	//@{
	void XMLCALL ElementStartHandle(void *data, const char *el, const char **attr);
	void XMLCALL ElementEndHandle(void *data, const char *el);
	void XMLCALL DataHandle(void* data, const XML_Char *s, int len);
	//@}

	/// @name Helper functions for string manipulation
	//@{
	void remControl(std::string& str);
	int StringSplit(string str, string delim, std::vector<string>& results);
	//@}

	/** This class is used to load and maintain XML resources. XML file is automatically parsed into pairs
	    node->value. Values are NOT stored with types, but as a raw text data. You have to know type of node to
	    be able to use it (Via template method GetAttribute). Remember that you should always get data from this
	    class only once, and store it in some appropriate variable/class (due to a necessary conversion from string
	    to output type, which is very slow)
	 */

	class XMLResource : public Resource
	{
	public:
		/// @name Specifies mode for Enter method (Absolute or Relative)
		enum eEnterMode { ABS, REL };

		virtual ~XMLResource(void);
		static ResourcePtr CreateMe(void);

		/** Return attribute casted to type T. 
		    name is full path to the node/attribute, starting at actual RootNode, which is inserted before
		    the name parameter. (root/name)
		 */
		template <typename T> T GetAttribute(const string& name);

		/** Navigate in tree. 
		    If mode is REL, set RootNode to RootNode/name
		    If mode is ABS, set RootNode to name
		    It does NOT check if the path is valid or not.
		 */
		bool Enter(const string& name, eEnterMode mode = REL);

		/// @name Return n levels up in the tree structure 
		bool Leave(const int n = 1);

		/// @name Friend classes used by Expat parser
		//@{
		friend void XMLCALL ResourceSystem::ElementStartHandle(void *data, const char *el, const char **attr);
		friend void XMLCALL ResourceSystem::ElementEndHandle(void *data, const char *el);
		friend void XMLCALL ResourceSystem::DataHandle(void* data, const XML_Char *s, int len);
		//@}


		/** Iterator class used to iterate over nodes in current level. Return all immediate values
		    Currently, you should only initialize iterator with XMLResource::begin()
		 */
		class node_iterator : public std::iterator<std::input_iterator_tag, string>
		{
		public:
			//node_iterator(const XMLDataMap::const_iterator node, XMLResourcePtr me);
			node_iterator(const XMLDataMap::const_iterator node, const XMLResource* me);
			~node_iterator(void);

			/// @name Check if the node is current-level node or not
			bool CheckNode(void);

			/// @name Common iterator functionality
			//@{
			node_iterator& operator=(const XMLResource::node_iterator& other);
			bool operator==(const node_iterator& other);
			bool operator!=(const node_iterator& other);
			node_iterator& operator++(int);
			//@}

			/// @name Get name of the attribute pointed by the iterator
			string GetName(void);
			/** Return value of type U of the attribute pointed by the iterator.
			    Uses GetAttribute<T> internally
			 */
			template <typename U> U Get(void);

		private:
			XMLDataMap::const_iterator mNodeIt;
			//const XMLResourcePtr me;
			const XMLResource* me;
		};

		/// @name Return iterator to the first node in current level (Same as standard STL iterators)
		const node_iterator begin(void) const;
		/// @name Return iterator to the behind-last node in current level (Same as standard STL iterators)
		const node_iterator end(void) const;

	protected:	
		/// @name container used to store data
		XMLDataMap mDataMap;
		/// @name stack to hold actual path in tree
		std::vector<string> mStack;
		/// @name buffer used for loading
		char* mBuff;
		/// @name node
		string mNode;
		/// @name root node
		string mRootNode;

		/// @name Load/Unload implementation
		//@{
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);	
		//@}
	};

	template <typename T> T XMLResource::GetAttribute(const string& name)
	{
		string str = mDataMap[mRootNode + name];
		assert(!str.empty() && "Value not found");
		T re;
		try {
			re = boost::lexical_cast<T>(str);
		} catch (boost::bad_lexical_cast &) {
			assert(false && "Cannot cast variable");
		}
		return re;
	}
}

#endif