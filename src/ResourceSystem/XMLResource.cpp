#include "Common.h"
#include "XMLResource.h"
#include "DataContainer.h"
#include <expat.h>
#include <boost/algorithm/string.hpp>

using namespace ResourceSystem;

struct XMLMapDataHolder
{
	XMLDataMap* mDataMapPtr;
	XMLDataMap::iterator mCurrentNode;
};

void XMLCALL ElementStartHandle(void *data, const XML_Char *el, const XML_Char **attr)
{
	XMLMapDataHolder* me = (XMLMapDataHolder*)data;
	me->mCurrentNode = me->mDataMapPtr->append_child(me->mCurrentNode, el);

	for (int i = 0; attr[i]; i += 2) {
		XMLDataMap::iterator child = me->mDataMapPtr->append_child(me->mCurrentNode, attr[i]);
		me->mDataMapPtr->append_child(child, attr[i+1]);
	}

}

void XMLCALL ElementEndHandle(void *data, const XML_Char *el)
{
	XMLMapDataHolder* me = (XMLMapDataHolder*)data;
	me->mCurrentNode = me->mDataMapPtr->parent(me->mCurrentNode);
}

void XMLCALL DataHandle(void* data, const XML_Char *s, int len)
{
	XMLMapDataHolder* me = (XMLMapDataHolder*)data;
	if (len > 0) 
	{
		XML_Char* buf = new XML_Char[len+1];
		memcpy(buf, s, len);
		buf[len] = '\0';
		string str = buf;
		str.erase(remove_if(str.begin(),str.end(),iscntrl),str.end());
		boost::trim(str);
		if (!str.empty())
		{
			me->mDataMapPtr->append_child(me->mCurrentNode, str);
		}
		delete[] buf;
	}
}

size_t XMLResource::LoadImpl(void)
{
	XML_Memory_Handling_Suite mmhs;
	mmhs.malloc_fcn = CustomMalloc;
	mmhs.free_fcn = CustomFree;
	mmhs.realloc_fcn = NULL;
	XML_Parser p = XML_ParserCreate_MM(NULL, &mmhs, NULL);
	OC_ASSERT(p);

	XMLMapDataHolder holder;
	mDataMap.clear();
	mTopNode = mDataMap.begin();
	holder.mDataMapPtr = &mDataMap;
	holder.mCurrentNode = mTopNode;
	XML_SetUserData(p, &holder);
	XML_SetElementHandler(p, ElementStartHandle, ElementEndHandle);
	XML_SetCharacterDataHandler(p, DataHandle);

	DataContainer cont;
	GetRawInputData(cont);

	if (XML_Parse(p, (char*)cont.GetData(), cont.GetSize(), true) == XML_STATUS_ERROR)
	{
		ocError << "XMLResource: Parse error at line " << (int32)XML_GetCurrentLineNumber(p) << ": " << (char*)XML_ErrorString(XML_GetErrorCode(p));
		XML_ParserFree(p);
		return 0;
	}

	// move the top level node one level down to the XML document element
	++mTopNode;

	XML_ParserFree(p);
	size_t dataSize = cont.GetSize();
	cont.Release();

	return dataSize;
}


bool XMLResource::UnloadImpl(void)
{
	mDataMap.clear();
	return true;
}

ResourcePtr XMLResource::CreateMe(void)
{
	return ResourcePtr(new XMLResource());
}


XMLResource::~XMLResource(void) {}

ResourceSystem::XMLNodeIterator ResourceSystem::XMLResource::IterateTopLevel( void )
{
	EnsureLoaded();
	return XMLNodeIterator(this, mDataMap.begin(mTopNode));
}

ResourceSystem::XMLNodeIterator ResourceSystem::XMLResource::IterateChildren( const XMLNodeIterator iter )
{
	EnsureLoaded();
	return XMLNodeIterator(this, mDataMap.begin(iter));
}

ResourceSystem::XMLNodeIterator ResourceSystem::XMLResource::EndTopLevel( void )
{
	return XMLNodeIterator(this, mDataMap.end(mTopNode));
}

ResourceSystem::XMLNodeIterator ResourceSystem::XMLResource::EndChildren( const XMLNodeIterator iter )
{
	return XMLNodeIterator(this, mDataMap.end(iter));
}
