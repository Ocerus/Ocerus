#include "Common.h"
#include "XMLResource.h"

using namespace ResourceSystem;
/*
Note: Code needs some cleanup
Im leaving some commented parts of code for future use
It will be removed in final version
Check demo in game.cpp to see some basic concept of using this class
*/


ResourcePtr XMLResource::CreateMe(void)
{
	return ResourcePtr(DYN_NEW XMLResource());
}

XMLResource::~XMLResource(void)
{
	UnloadImpl();
}


bool XMLResource::LoadImpl(void)
{
	XML_Parser p = XML_ParserCreate(NULL);
	InputStream& is = OpenInputStream(ISM_TEXT);
	mBuff = new char[BUFFSIZE];

	if (!p)
	{
		gLogMgr.LogMessage("XMLResource: Couldn't allocate memory for parser", LOG_ERROR);
		CloseInputStream();
		XML_ParserFree(p);
		return false;
	}
	XML_SetUserData(p, this);
	XML_SetElementHandler(p, ElementStartHandle, ElementEndHandle);
	XML_SetCharacterDataHandler(p, DataHandle);
	for (;;)
	{
		int done;
		int len;

		is.read(mBuff, BUFFSIZE-1);
		if (!is.gcount() && is.fail())
		{
			gLogMgr.LogMessage("XMLResource: Read Error", LOG_ERROR);
			CloseInputStream();
			XML_ParserFree(p);
			return false;
		}

		len = is.gcount();
		mBuff[len] = '\0';
		
		done = !is.good();

		if (XML_Parse(p, mBuff, len, done) == XML_STATUS_ERROR)
		{
			std::stringstream ss;
			ss << "XMLResource: Parse error at line " << XML_GetCurrentLineNumber(p) << ": " << XML_ErrorString(XML_GetErrorCode(p));
			gLogMgr.LogMessage(ss.str(), LOG_ERROR);
			CloseInputStream();
			XML_ParserFree(p);
			return false;
		}

		if (done) 
		{
			break;
		}
	}
	CloseInputStream();
	XML_ParserFree(p);
	delete mBuff;
	mRootNode = "";
	mStack.clear();
	return true;
}


bool XMLResource::UnloadImpl(void)
{
	mDataMap.clear();
	return true;
}

bool XMLResource::Enter(const string& name, eEnterMode mode)
{
	if (mode == REL)
	{
		if (!name.empty())
		{
			mRootNode = mRootNode + name + "/";
			std::vector<string> spl;
			int n = StringSplit(name, "/", spl);
			for (std::vector<string>::iterator it = spl.begin(); it != spl.end(); it++)
			{
				mStack.push_back((*it));
			}
		}
	}
	else 
	{
		if (!name.empty()) {
			mRootNode = name + "/";
			mStack.clear();
			std::vector<string> spl;
			int n = StringSplit(name, "/", spl);
			for (std::vector<string>::iterator it = spl.begin(); it != spl.end(); it++)
			{
				mStack.push_back((*it));
			}
		}
		else
		{
			mRootNode.clear();
			mStack.clear();
		}
		
	}
	//gLogMgr.LogMessage("XML: rootnode is: " + mRootNode);
	return true;
}

bool XMLResource::Leave(const int n)
{
	for (int i = 0; i < n; i++)
	{
		if (!mStack.empty()) {
			mStack.pop_back();
		}
	}

	mRootNode.clear();
	for (std::vector<string>::iterator it = mStack.begin(); it != mStack.end(); it++) 
	{
		mRootNode = mRootNode + (*it) + "/";
	}
	//gLogMgr.LogMessage("XML: rootnode is: " + mRootNode);
	return true;
}

/*
class node_iterator
		{
		public:
			node_iterator(XMLDataMap::iterator p);
			~node_iterator();

			node_iterator& operator=(const node_iterator& other);
			bool operator==(const node_iterator& other);
			bool operator!=(const node_iterator& other);
			node_iterator& operator++()


		private:
			XMLDataMap::iterator mNode;	
		};

		node_iterator begin();
		node_iterator end();
		*/
/* 
  ITERATOR
*/

//XMLResource::node_iterator::node_iterator(const XMLDataMap::const_iterator node, XMLResourcePtr me)
XMLResource::node_iterator::node_iterator(const XMLDataMap::const_iterator node, const XMLResource* me)
{
	mNodeIt = node;
	//*(this->me) = *me;
	this->me = me;
}

XMLResource::node_iterator::~node_iterator(void) 
{
	//gLogMgr.LogMessage("ITERATOR DESTROIED: " + me->mRootNode);
}

XMLResource::node_iterator& XMLResource::node_iterator::operator=(const XMLResource::node_iterator& other)
{
	this->mNodeIt = other.mNodeIt;
	//XMLResourcePtr p = other.me;
	//*(this->me) = *p;
	//*(this->me) = *other.me;
	this->me = other.me;
	return (*this);
}

bool XMLResource::node_iterator::operator==(const XMLResource::node_iterator& other)
{
	return (mNodeIt == other.mNodeIt);
}

bool XMLResource::node_iterator::operator!=(const XMLResource::node_iterator& other)
{
	return (mNodeIt != other.mNodeIt);
}

bool XMLResource::node_iterator::CheckNode(void)
{
	string key = (*mNodeIt).first;		
	int len = (me->mRootNode).length();
	if ((key.substr(0,len)).compare(me->mRootNode) == 0)
	{
		if ((key.substr(len)).find_first_of("/") == string::npos)
		{
			//gLogMgr.LogMessage("ITERATOR++ key" + key);
			//gLogMgr.LogMessage("ITERATOR++ KEY SUB + 1 " + (key.substr(len)));
			//return (*this);
			return true;
		}
	}
	return false;
}

XMLResource::node_iterator& XMLResource::node_iterator::operator++(int)
{
	while (++mNodeIt != me->mDataMap.end())
	{
		if(CheckNode())
		{
			return (*this);
		}
	}
	//gLogMgr.LogMessage("ITERATOR++ DONE, RETURNING END");
	*this = me->end();
	return (*this);
}

//string XMLResource::node_iterator::operator*()
//{
//	return ((*mNodeIt).second);
//}

string XMLResource::node_iterator::GetName(void)
{	
	return ((mNodeIt->first).substr((mNodeIt->first).find_last_of("/") + 1));
}

template <typename U> U XMLResource::node_iterator::Get(void)
{
	return XMLResource::GetAttribute<U>(mNodeIt->first);
}


const XMLResource::node_iterator XMLResource::begin(void) const //, use when using normal pointer
{
	//const XMLResourcePtr p = XMLResourcePtr(this);
	//XMLResource::node_iterator re = XMLResource::node_iterator(mDataMap.begin(),p);
	//XMLResource::node_iterator re = XMLResource::node_iterator(mDataMap.begin(), XMLResourcePtr(this));
	XMLResource::node_iterator re = XMLResource::node_iterator(mDataMap.begin(),this);
	if (re.CheckNode())
	{
		return re;
	}
	else 
	{
		return re++;
	}
}

const XMLResource::node_iterator XMLResource::end(void) const //, use when using normal pointer
{
	//return (XMLResource::node_iterator(mDataMap.end(),XMLResourcePtr(this)));
	return (XMLResource::node_iterator(mDataMap.end(),this));
}



void ResourceSystem::remControl(std::string& str)
{
	str.erase(remove_if(str.begin(),str.end(),iscntrl),str.end());
}

void XMLCALL ResourceSystem::ElementStartHandle(void *data, const char *el, const char **attr)
{
	XMLResource* me = (XMLResource*)data;
	string str = el;
	me->mStack.push_back(str);
	me->mNode = me->mNode + str + "/";

	for (int i = 0; attr[i]; i += 2) {
		me->mDataMap.insert(std::pair<string, string>(me->mNode + attr[i], attr[i+1]));
	}
	
}

void XMLCALL ResourceSystem::ElementEndHandle(void *data, const char *el)
{
  //std::cout << "END" << std::endl;
	XMLResource* me = (XMLResource*)data;
	me->mStack.pop_back();
	//string str = el;
	me->mNode.clear();
	for (std::vector<string>::iterator it = me->mStack.begin(); it != me->mStack.end(); it++) 
	{
		me->mNode = me->mNode + (*it) + "/";
	}
}

void XMLCALL ResourceSystem::DataHandle(void* data, const XML_Char *s, int len)
{
	//std::cout << "DATA" << std::endl;
	XMLResource* me = (XMLResource*)data;
	if (len > 0) 
	{
		XML_Char* buf = (XML_Char*)malloc(sizeof(XML_Char)*(len+1));
		memcpy(buf, s, len);
		buf[len] = '\0';
		string str = buf;
		remControl(str);
		if (!str.empty()) {
			me->mNode.erase(me->mNode.end()-1);
			//gLogMgr.LogMessage("XML LOADING -- Node: " + me->mNode);
			me->mDataMap.insert(std::pair<string, string>(me->mNode, str));
			me->mNode += "/";
		}
		delete buf;
	}
}

int ResourceSystem::StringSplit(string str, string delim, std::vector<string>& results)
{
	int cutAt;
	while((cutAt = str.find_first_of(delim)) != str.npos)
	{
		if(cutAt > 0)
		{
			results.push_back(str.substr(0,cutAt));
		}
		str = str.substr(cutAt+1);
	}
	if(str.length() > 0)
	{
		results.push_back(str);
	}
	return results.size();
}