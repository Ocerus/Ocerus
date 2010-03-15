/// @file
/// Store XML data to file.

#ifndef _XMLOUTPUT_H_
#define _XMLOUTPUT_H_

#include "Base.h"
#include <boost/filesystem/fstream.hpp>

namespace ResourceSystem
{
	class XMLOutput
	{
	public:
		XMLOutput(const string& fileName, const string& indentStr = "  ");
		~XMLOutput(void);

		void BeginElementStart(const string& name);
		void AddAttribute(const string& key, const string& value);
		void BeginElementFinish();
		inline void BeginElement(const string& name) { BeginElementStart(name); BeginElementFinish(); }
		void WriteNewLine();
		void WriteString(const string& str);
		void EndElement();
	private:
		boost::filesystem::ofstream mOutStream;
		stack<string> mElementStack;
		bool mElementOpen;
		bool mElementLast;
		string mIndentStr;
	};
}

#endif