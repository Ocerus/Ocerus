/// @file
/// Store XML data to file.

#ifndef _XMLOUTPUT_H_
#define _XMLOUTPUT_H_

#include "Base.h"
#include <boost/filesystem/fstream.hpp>

namespace ResourceSystem
{
	/// Provides a formatted XML output to file.
	/// The class remembers the element order so it automatically writes the appropriate end element tags.
	class XMLOutput
	{
	public:
		/// Opens the file for writting XML output and writes the header.
		/// @param fileName The name of the output file.
		/// @param indentStr String that is used for indenting XML data.
		XMLOutput(const string& fileName, const string& indentStr = "  ");
		
		/// Closes all open elements and closes the file.
		~XMLOutput(void);

		/// Starts the begin element tag (writes <name ).
		/// It is possible to add an attribute by AddAttribute method or
		/// to finish the start element by BeginElementFinish method.
		/// @param name The name of the element.
		void BeginElementStart(const string& name);

		/// Adds attribute to the begin element tag (writes key="value").
		/// Must be call only between BeginElementStart and BeginElementFinish methods.
		/// @param key The name of the attribute.
		/// @param value The value of the attribute.
		void AddAttribute(const string& key, const string& value);

		/// Finishes the begin element tag (writes >).
		/// Must be call after BeginElementStart method.
		void BeginElementFinish();

		/// Writes the begin element tag without attributes (writes <name>).
		/// Same as BeginElementStart(name); BeginElementFinish();
		/// @param name The name of the element.
		inline void BeginElement(const string& name) { BeginElementStart(name); BeginElementFinish(); }
		
		/// Starts a new line with an appropriate indentation.
		void WriteNewLine();
		
		/// Writes a string data to XML.
		/// Do not use for writing element tags or new lines.
		/// @param str The string to write.
		void WriteString(const string& str);
		
		/// Writes the appropriate end element tag (writes </name>).
		void EndElement();
	
	private:
		/// The output stream to which the methods write.
		boost::filesystem::ofstream mOutStream;
		
		/// The stack to remember the started elements.
		stack<string> mElementStack;
		
		/// Indicates whether the BeginElementStart method is called but no BeginElementFinish yet.
		bool mElementOpen;
		
		/// Indicated whether the there is no element closing after the last element opening.
		bool mElementLast;

		/// The string for the indentation of element tags.
		string mIndentStr;
	};
}

#endif