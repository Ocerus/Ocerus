/// @file
/// Implementation of a resource able to read arbitrary XML files.

#ifndef _XMLRESOURCE_H_
#define _XMLRESOURCE_H_

#include "Base.h"

namespace ResourceSystem 
{
	/// Container for data stored in this resource.
	typedef tree<string> XMLDataMap;

	/// @brief This class is used to load and maintain XML resources.
	/// @remarks The XML file is automatically parsed into the pairs of
	/// node->value. Values are NOT stored typed, but as a raw text data. You have to know the type of a node to
	/// be able to use it. Remember that you should always get data from this
	/// class once only, and store it in an appropriate variable/class due to the necessary conversion from string
	/// to another type, which can be very slow.
	class XMLResource : public Resource
	{
	public:

		virtual ~XMLResource(void);

		/// Factory function.
		static ResourcePtr CreateMe(void);

		/// This iterator serves to go through nodes on one level of the XML tree.
		class NodeIterator : public XMLDataMap::sibling_iterator
		{
		public:

			/// Returns the value of an attribute of the current node.
			template<typename T>
			inline T GetAttribute(const string& name) { return mOwner->GetAttribute<T>(*this, name); }

			/// Returns the value of the current node.
			template<typename T>
			T GetValue(void) { return mOwner->GetValue<T>(*this); }

			/// Returns the value of the first child of the current node.
			template<typename T>
			T GetChildValue(void) { return mOwner->GetChildValue<T>(*this); }

			/// Copy constructor.
			NodeIterator(const NodeIterator& rhs) { operator=(rhs); }

			/// Copy operator.
			NodeIterator& operator=(const NodeIterator& rhs)
			{
				XMLDataMap::sibling_iterator::operator=(rhs);
				mOwner = rhs.mOwner;
				return *this;
			}

		private:
			friend class XMLResource;

			/// Private ctor for internal use by XMLResource.
			NodeIterator(XMLResource* xml, const XMLDataMap::sibling_iterator iter): 
			   mOwner(xml), XMLDataMap::sibling_iterator(iter) {}

			NodeIterator(void) {}

			XMLResource* mOwner;
		};

		/// Returns a node iterator to the top level nodes of the XML document.
		NodeIterator IterateTopLevel(void);

		/// End iterator for IterateTopLevel.
		NodeIterator EndTopLevel(void);

		/// Returns a node iterator for children of the specified node.
		NodeIterator IterateChildren(const NodeIterator iter);

		/// End iterator for IterateChildren.
		NodeIterator EndChildren(const NodeIterator iter);

		/// Returns the value of an attribute of the specified node.
		template<typename T>
		T GetAttribute(const NodeIterator iter, const string& attributeName)
		{
			XMLDataMap::sibling_iterator attr = find(mDataMap.begin(iter), mDataMap.end(iter), attributeName);
			if (attr == mDataMap.end(iter))
				return StringConverter::FromString<T>("");
			XMLDataMap::sibling_iterator data = mDataMap.begin(attr);
			if (data == mDataMap.end(attr))
				return StringConverter::FromString<T>("");
			return StringConverter::FromString<T>(*data);
		}

		/// Returns the value of the specified node.
		template<typename T>
		T GetValue(const NodeIterator iter)
		{
			return StringConverter::FromString(*iter);
		}

		/// Returns the value of the first child of the specified node.
		template<typename T>
		T GetChildValue(const NodeIterator iter)
		{
			XMLDataMap::sibling_iterator child = mDataMap.begin(iter);
			if (child == mDataMap.end(iter))
				return StringConverter::FromString<T>("");
			return StringConverter::FromString<T>(*child);
		}

	protected:

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);	

	private:

		/// Container used to store the parsed XML data.
		XMLDataMap mDataMap;

		/// Top level node iterator to be used as a reference value.
		XMLDataMap::iterator mTopNode;
	};

}

#endif