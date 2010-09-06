/// @file
/// Implementation of a resource able to read arbitrary XML files.

#ifndef _XMLRESOURCE_H_
#define _XMLRESOURCE_H_

#include "Base.h"
#include "../ResourceSystem/Resource.h"

namespace ResourceSystem
{
	/// Container for data stored in this resource.
	typedef tree<string> XMLDataMap;

	/// This iterator serves to go through nodes on one level of the XML tree.
	class XMLNodeIterator : public XMLDataMap::sibling_iterator
	{
	public:
		
		/// Returns a node iterator for children of the specified node.
		inline XMLNodeIterator IterateChildren(void);

		/// End iterator for IterateChildren.
		inline XMLNodeIterator EndChildren(void);
		
		/// Returns true if the attribute of the current node exists.
		inline bool HasAttribute(const string& name);

		/// Returns the value of an attribute of the current node.
		template<typename T>
		inline T GetAttribute(const string& name);

		/// Returns the value of the current node.
		template<typename T>
		T GetValue(void);

		/// Returns the value of the first child of the current node.
		template<typename T>
		T GetChildValue(void);

		/// Copy constructor.
		XMLNodeIterator(const XMLNodeIterator& rhs): XMLDataMap::sibling_iterator(rhs) { operator=(rhs); }

		/// Copy operator.
		XMLNodeIterator& operator=(const XMLNodeIterator& rhs)
		{
			XMLDataMap::sibling_iterator::operator=(rhs);
			mOwner = rhs.mOwner;
			return *this;
		}

	private:

		friend class XMLResource;

		/// Private ctor for internal use by XMLResource.
		XMLNodeIterator(XMLResource* xml, const XMLDataMap::sibling_iterator iter):
		XMLDataMap::sibling_iterator(iter), mOwner(xml) {}

		XMLNodeIterator(void) {}

		XMLResource* mOwner;
	};


	/// This class is used to load and maintain XML resources.
	/// The XML file is automatically parsed into the pairs of
	/// node->value. Values are NOT stored typed, but as a raw text data. You have to know the type of a node to
	/// be able to use it. Remember that you should always get data from this
	/// class once only, and store it in an appropriate variable/class due to the necessary conversion from string
	/// to another type, which can be very slow.
	class XMLResource : public Resource
	{
	public:

		/// Virtual destructor.
		virtual ~XMLResource(void);

		/// Factory function.
		static ResourcePtr CreateMe(void);

		/// Returns a node iterator to the top level nodes of the XML document.
		XMLNodeIterator IterateTopLevel(void);

		/// End iterator for IterateTopLevel.
		XMLNodeIterator EndTopLevel(void);

		/// Returns a node iterator for children of the specified node.
		XMLNodeIterator IterateChildren(const XMLNodeIterator iter);

		/// End iterator for IterateChildren.
		XMLNodeIterator EndChildren(const XMLNodeIterator iter);

		/// Returns true if the attribute of the specified node exists.
		bool HasAttribute(const XMLNodeIterator iter, const string& attributeName)
		{
			XMLDataMap::sibling_iterator attr = find(mDataMap.begin(iter), mDataMap.end(iter), attributeName);
			return attr != mDataMap.end(iter);
		}

		/// Returns the value of an attribute of the specified node.
		template<typename T>
		T GetAttribute(const XMLNodeIterator iter, const string& attributeName)
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
		T GetValue(const XMLNodeIterator iter)
		{
			return StringConverter::FromString<T>(*iter);
		}

		/// Returns the value of the first child of the specified node.
		template<typename T>
		T GetChildValue(const XMLNodeIterator iter)
		{
			if (mDataMap.number_of_children(iter) == 0) return StringConverter::FromString<T>("");

			// we need to skip all the attributes here
			XMLDataMap::sibling_iterator child = mDataMap.child(iter, mDataMap.number_of_children(iter) - 1);
			if (child == mDataMap.end(iter)) return StringConverter::FromString<T>("");
			else return StringConverter::FromString<T>(*child);
		}

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_XMLRESOURCE; }

	protected:

		virtual size_t LoadImpl(void);
		virtual bool UnloadImpl(void);

	private:

		/// Container used to store the parsed XML data.
		XMLDataMap mDataMap;

		/// Top level node iterator to be used as a reference value.
		XMLDataMap::iterator mTopNode;
	};


	inline XMLNodeIterator XMLNodeIterator::IterateChildren(void)
	{
		return mOwner->IterateChildren(*this);
	}
	
	inline XMLNodeIterator XMLNodeIterator::EndChildren(void)
	{
		return mOwner->EndChildren(*this);
	}
	
	inline bool XMLNodeIterator::HasAttribute( const string& name )
	{
		return mOwner->HasAttribute(*this, name);
	}

    template<typename T>
	inline T XMLNodeIterator::GetAttribute(const string& name)
    {
        return mOwner->GetAttribute<T>(*this, name);
    }
	template<typename T>
	T XMLNodeIterator::GetValue(void)
    {
        return mOwner->GetValue<T>(*this);
    }

	template<typename T>
	T XMLNodeIterator::GetChildValue(void)
    {
        return mOwner->GetChildValue<T>(*this);
    }
}

#endif
