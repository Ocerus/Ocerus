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
		//FIXME neco udelat s tim, at to tady zbytecne nealokuju a neprevadim na string...je to kvuli tomu, ze s neni ukoncene nulou
		XML_Char* buf = new XML_Char[len+1];
		memcpy(buf, s, len);
		buf[len] = '\0';
		string str = buf;
		str.erase(remove_if(str.begin(),str.end(),iscntrl),str.end()); // remove contrl sequence chars
		boost::trim(str);
		if (!str.empty())
			me->mDataMapPtr->append_child(me->mCurrentNode, str);
		delete[] buf;
	}
}

bool XMLResource::LoadImpl(void)
{
	XML_Parser p = XML_ParserCreate(NULL);
	BS_ASSERT(p);

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
		gLogMgr.LogMessage("XMLResource: Parse error at line ", (int32)XML_GetCurrentLineNumber(p), ": ", (char*)XML_ErrorString(XML_GetErrorCode(p)), LOG_ERROR);
		XML_ParserFree(p);
		return false;
	}

	// move the top level node one level down to the XML document element
	++mTopNode;

	XML_ParserFree(p);
	delete[] cont.GetData();
	return true;
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

ResourceSystem::XMLResource::NodeIterator ResourceSystem::XMLResource::IterateTopLevel( void )
{
	return NodeIterator(this, mDataMap.begin(mTopNode));
}

ResourceSystem::XMLResource::NodeIterator ResourceSystem::XMLResource::IterateChildren( const NodeIterator iter )
{
	return NodeIterator(this, mDataMap.begin(iter));
}

ResourceSystem::XMLResource::NodeIterator ResourceSystem::XMLResource::EndTopLevel( void )
{
	return NodeIterator(this, mDataMap.end(mTopNode));
}

ResourceSystem::XMLResource::NodeIterator ResourceSystem::XMLResource::EndChildren( const NodeIterator iter )
{
	return NodeIterator(this, mDataMap.end(iter));
}