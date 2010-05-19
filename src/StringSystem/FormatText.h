/// @file
/// 

#ifndef FormatText_h__
#define FormatText_h__

#include "Base.h"
#include "TextData.h"

namespace StringSystem
{
	/// Helps to replace variable sequences in the text by another text.
	/// Possible usage: FormatText(variableTextData) << firstVariable << secondVariable;
	class FormatText
	{
	public:
		/// Wild character.
		static const char WildChar = '%';

		/// Constructs new empty parameters.
		FormatText(const TextData& text) : mTextData(text) {}

		/// Assignment operator.
		FormatText& operator=(const FormatText& rhs);

		/// Replaces the variable sequence with a minimal number by the text in the parameter.
		/// @param newText The text that replace the variable sequence. 
		FormatText& operator<<(const TextData& newText);

		/// Returns the edited text.
		operator const TextData& (void) const;

	private:
		/// Finds a position of the variable sequence with a minimal number.
		TextData::size_type FindMinVariable(void);

		/// Text that is being edited.
		TextData mTextData;
	};
}

#endif // FormatText_h__