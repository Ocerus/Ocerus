#ifndef _XMLRESOURCE_H_
#define _XMLRESOURCE_H_

#include "Resource.h"
#include "../Utility/Settings.h"
#include "../Utility/ResourcePointers.h"
#include "../Utility/Tree.h"
#include "../Utility/StringConverter.h"
#include <algorithm>

namespace ResourceSystem 
{
	/// @name define a container for textdata
	typedef tree<string> XMLDataMap;

	/** @name This class is used to load and maintain XML resources. XML file is automatically parsed into pairs
	    node->value. Values are NOT stored with types, but as a raw text data. You have to know type of node to
	    be able to use it. Remember that you should always get data from this
	    class only once, and store it in some appropriate variable/class (due to a necessary conversion from string
	    to output type, which is very slow)
	 */
	class XMLResource : public Resource
	{
	public:

		virtual ~XMLResource(void);
		static ResourcePtr CreateMe(void);

		/// @name This iterator serves to go through nodes on one level of the XML tree.
		class NodeIterator : public XMLDataMap::sibling_iterator
		{
		public:
			/// @name Returns an attribute of specified name and type of the node the iterator points to.
			template<typename T>
			inline T GetAttribute(const string& name) { return mOwner->GetAttribute<T>(*this, name); }

			/// @name Returns the of the node the iterator points to.
			template<typename T>
			T GetValue(void) { return mOwner->GetValue<T>(*this); }

			/// @name Returns the of the child of the node the iterator points to.
			template<typename T>
			T GetChildValue(void) { return mOwner->GetChildValue<T>(*this); }

			/// @name Copy ctor.
			NodeIterator(const NodeIterator& rhs) { operator=(rhs); }

			/// @name Copy operator.
			NodeIterator& operator=(const NodeIterator& rhs)
			{
				XMLDataMap::sibling_iterator::operator=(rhs);
				mOwner = rhs.mOwner;
				return *this;
			}

		private:
			friend class XMLResource;

			/// @name Private ctor for internal use by XMLResource.
			NodeIterator(XMLResource* xml, const XMLDataMap::sibling_iterator iter): 
			   mOwner(xml), XMLDataMap::sibling_iterator(iter) {}

			NodeIterator(void) {}

			XMLResource* mOwner;
		};

		/// @name Returns a node iterator to the top level nodes of the XML document.
		NodeIterator IterateTopLevel(void);

		/// @name End iterator for the IterateTopLevel.
		NodeIterator EndTopLevel(void);

		/// @name Returns a node iterator for children of the specified node.
		NodeIterator IterateChildren(const NodeIterator iter);

		/// @name End iterator for IterateChildren.
		NodeIterator EndChildren(const NodeIterator iter);

		/// @name Returns an attribute of specified name and type of the specified node.
		template<typename T>
		T GetAttribute(const NodeIterator iter, const string& name)
		{
			XMLDataMap::sibling_iterator attr = find(mDataMap.begin(iter), mDataMap.end(iter), name);
			if (attr == mDataMap.end(iter))
				return StringConverter::FromString<T>("");
			XMLDataMap::sibling_iterator data = mDataMap.begin(attr);
			if (data == mDataMap.end(attr))
				return StringConverter::FromString<T>("");
			return StringConverter::FromString<T>(*data);
		}

		/// @name Returns the of the specified node.
		template<typename T>
		T GetValue(const NodeIterator iter)
		{
			return StringConverter::FromString(*iter);
		}

		/// @name Returns the of the child of the specified node.
		template<typename T>
		T GetChildValue(const NodeIterator iter)
		{
			XMLDataMap::sibling_iterator child = mDataMap.begin(iter);
			if (child == mDataMap.end(iter))
				return StringConverter::FromString<T>("");
			return StringConverter::FromString<T>(*child);
		}

	protected:	
		/// @name Load/Unload implementation
		//@{
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);	
		//@}

	private:
		/// @name Container used to store data.
		XMLDataMap mDataMap;

		/// @name Top level node iterator to be used as a reference value.
		XMLDataMap::iterator mTopNode;
	};

}

#endif