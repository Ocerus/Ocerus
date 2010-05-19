#include "Common.h"
#include "XMLOutput.h"

using namespace ResourceSystem;

XMLOutput::XMLOutput(const string& fileName, const string& indentStr)
  : mOutStream(), mElementStack(), mElementOpen(false), mElementLast(false), mIndentStr(indentStr)
{
	mOutStream.open(fileName.c_str());
	mOutStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
}

XMLOutput::~XMLOutput()
{
	CloseAndReport();
}

bool XMLOutput::CloseAndReport()
{
	if (!mOutStream.is_open()) { return false; }
	while (!mElementStack.empty()) { EndElement(); }
	bool result = mOutStream.good();
	mOutStream.clear();
	mOutStream.close();
	return result && mOutStream.good();
}

void XMLOutput::BeginElementStart(const string& name)
{
	OC_ASSERT(!mElementOpen);
	WriteNewLine();
	mOutStream << "<" << name;
	mElementOpen = true;
	mElementStack.push(name);
}

void XMLOutput::AddAttribute(const string& key, const string& value)
{
	OC_ASSERT(mElementOpen);
	mOutStream << " " << key << "=\"" << value << "\"";
}

void XMLOutput::BeginElementFinish()
{
	OC_ASSERT(mElementOpen);
	mOutStream << ">";
	mElementOpen = false;
	mElementLast = true;
}

void XMLOutput::EndElement()
{
	OC_ASSERT(!mElementOpen && !mElementStack.empty());
	string name = mElementStack.top();
	mElementStack.pop();
	if (!mElementLast) { WriteNewLine(); }
	mOutStream << "</" << name << ">";
	mElementLast = false;
}

void XMLOutput::WriteNewLine()
{
	mOutStream << std::endl;
	for (uint32 i = 0; i < mElementStack.size(); ++i)
	{
		mOutStream << mIndentStr;
	}
}

void XMLOutput::WriteString(const string &str)
{
	mOutStream << str;
}